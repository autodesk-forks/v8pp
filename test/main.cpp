#include <iostream>
#include <vector>
#include <exception>
#include <cstdlib>

#include <v8.h>

#include <boost/program_options.hpp>

#include <v8pp/context.hpp>

int main(int argc, char const * argv[])
{
	typedef std::vector<std::string> script_names;
	script_names scripts;
	std::string lib_path;

	namespace po = boost::program_options;
	po::options_description desc("allowed options");
	po::positional_options_description positionals;
	positionals.add("script", -1);
	desc.add_options()
		("help,h", "print this help message.")
		("script,s", po::value(&scripts), "script to run.")
		("library-path,l", po::value(&lib_path), "path containing plugin libraries.")
		;

	po::variables_map vm;
	try
	{
		po::store(po::command_line_parser(argc, argv).options(desc).positional(positionals).run(), vm);
	}
	catch (po::error const& ex)
	{
		std::cerr << ex.what() << '\n' << desc << std::endl;
		return EXIT_FAILURE;
	}

	if ( vm.count("help") )
	{
		std::cout << desc << std::endl;
		return EXIT_SUCCESS;
	}

	po::notify(vm);

	if ( lib_path.empty() )
	{
		lib_path = V8PP_PLUGIN_LIB_PATH;
	}

	if ( scripts.empty() )
	{
		std::cerr << "must supply at least one script\n" << desc << std::endl;
		return EXIT_FAILURE;
	}

	try
	{
		v8pp::context ctx(lib_path);
		for (script_names::const_iterator it = scripts.begin(), end = scripts.end(); it != end; ++it)
		{
			v8::HandleScope scope;
			std::string const& script = *it;
			ctx.run(script.c_str());
		}
	}
	catch (std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	v8::V8::Dispose();
	return EXIT_SUCCESS;
}