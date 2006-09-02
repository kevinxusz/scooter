/*
 * This file was automaticly generated by clo++ version 0.6.4 (REL-0_6_4)
 * http://pmade.org/pjones/software/clo++/
 */

/** @file
 * This file contains the implementation of the parser class.
 * You can control the name of this source file using the
 * cxx_source_file variable.
**/

#include "dgd\Debug\dgDebugOpt.h"
#include <cstring>
#include <cstdlib>
#include <cctype>

namespace {
    const char const_usage[] =
"  --trace-allow-word-wrap bool     Enable word wrap\n"
"  --trace-allow-wrap bool          Enable character wrap\n"
"  --trace-enable                   Enable tracing output.\n"
"  --trace-help                     Print help message on DGD command line\n"
"                                   options and exit\n"
"  --trace-indent-step int          Default indentation step\n"
"  --trace-main-file string         Default log file\n"
"  --trace-max-width int            Default maximum output width\n"
"  --trace-min-width int            Default minimum output width\n"
"  --trace-space-characters string  Word Separator Characters\n"
"  --trace-turn-off string          Regexp on Channels which need to be\n"
"                                   turned off\n"
"  --trace-turn-on string           Regexp on Channels which need to be\n"
"                                   turned on\n"
"  --trace-version                  Print DGD version and exit\n";

    const char const_help_comment[] =
"use the -h option for help";

    const char* expand_long_name (const std::string &name);
}

//#########################################################################
DGD::parser::parser (void)
    : state_(state_option)
{
    std::memset(&locations_, 0, sizeof(locations_));
}
//#########################################################################
void DGD::parser::parse (int argc, char *argv[], bool call_finalize) {
    for (int i=1; i<argc; ++i) parse_element(argv[i], i, source_cl);
    if (call_finalize) finalize();
}
//#########################################################################
void DGD::parser::finalize (void) {
    if (state_ == state_value) {
	switch (openum_) {
	    case option_trace_allow_word_wrap:
		throw option_error("missing value for 'trace-allow-word-wrap' option");
	    case option_trace_allow_wrap:
		throw option_error("missing value for 'trace-allow-wrap' option");
	    case option_trace_enable:
		throw option_error("missing value for 'trace-enable' option");
	    case option_trace_help:
		throw option_error("missing value for 'trace-help' option");
	    case option_trace_indent_step:
		throw option_error("missing value for 'trace-indent-step' option");
	    case option_trace_main_file:
		throw option_error("missing value for 'trace-main-file' option");
	    case option_trace_max_width:
		throw option_error("missing value for 'trace-max-width' option");
	    case option_trace_min_width:
		throw option_error("missing value for 'trace-min-width' option");
	    case option_trace_space_characters:
		throw option_error("missing value for 'trace-space-characters' option");
	    case option_trace_turn_off:
		throw option_error("missing value for 'trace-turn-off' option");
	    case option_trace_turn_on:
		throw option_error("missing value for 'trace-turn-on' option");
	    case option_trace_version:
		throw option_error("missing value for 'trace-version' option");
	}

    }

}
//#########################################################################
void DGD::parser::parse_element (const char *element, int position, opsource source) {
    std::size_t length = std::strlen(element);

    switch (state_) {
	case state_consume:
	    non_options_.push_back(element);
	    break;
	case state_option:
	    if (length >= 2 && element[0] == '-' && element[1] == '-') {
		if (length == 2) { state_ = state_consume; return; }
		element += 2;
		const char *value = element;
		while (*value != 0 && *value != '=') ++value;
		if (*value == '=') {
		    std::string selement(element, value - element), svalue(++value);

		    parse_long_option(selement.c_str(), position, source);
		    if (state_ != state_value) {
			std::string error("the '"); error += element; error += "' option does not take a value";
			throw option_error(error);
		    }

		    parse_value(svalue.c_str());
		    state_ = state_option;
		} else {
		    parse_long_option(element, position, source);
		}
	    } else if (length >= 2 && element[0] == '-') {
		++element;

		if (length > 2) {
		    while (*element != 0) {
			parse_short_option(*element, position, source);
			++element;
	    
			if (state_ == state_value && *element != 0) {
			    parse_value(element);
			    state_ = state_option;
			    break;
			}
		    }
		} else {
		    parse_short_option(*element, position, source);
		}
	    } else {
		non_options_.push_back(element);
	    }
	    break;
	case state_value:
	    parse_value(element);
	    state_ = state_option;
	    break;
    }
}
//#########################################################################
void DGD::parser::parse_short_option (char option, int position, opsource source) {
    switch (option) {
    	case 'h':
    	    if (source != source_cl) break;
    	    throw autoexcept(autothrow_help, const_usage);
    	case '?':
    	    if (source != source_cl) break;
    	    throw autoexcept(autothrow_help, const_usage);
    }

    std::string error("unknown option: '"); error += option; error += "'";
    throw option_error(error);
}
//#########################################################################
void DGD::parser::parse_long_option (const char *option, int position, opsource source) {
    option = expand_long_name(option);

	    if (std::strcmp(option, "trace-allow-word-wrap") == 0) {
		source = source; // kill compiler unused variable warning
		if (locations_.trace_allow_word_wrap) {
		    throw option_error("the 'trace-allow-word-wrap' option is only allowed once");
		}
		openum_ = option_trace_allow_word_wrap;
		locations_.trace_allow_word_wrap = position;
		state_ = state_value;
		return;
	    } else if (std::strcmp(option, "trace-allow-wrap") == 0) {
		source = source; // kill compiler unused variable warning
		if (locations_.trace_allow_wrap) {
		    throw option_error("the 'trace-allow-wrap' option is only allowed once");
		}
		openum_ = option_trace_allow_wrap;
		locations_.trace_allow_wrap = position;
		state_ = state_value;
		return;
	    } else if (std::strcmp(option, "trace-enable") == 0) {
		source = source; // kill compiler unused variable warning
		if (locations_.trace_enable) {
		    throw option_error("the 'trace-enable' option is only allowed once");
		}
		openum_ = option_trace_enable;
		locations_.trace_enable = position;
		options_.trace_enable = !options_.trace_enable;
		return;
	    } else if (std::strcmp(option, "trace-help") == 0) {
		source = source; // kill compiler unused variable warning
		if (locations_.trace_help) {
		    throw option_error("the 'trace-help' option is only allowed once");
		}
		openum_ = option_trace_help;
		locations_.trace_help = position;
		options_.trace_help = !options_.trace_help;
		return;
	    } else if (std::strcmp(option, "trace-indent-step") == 0) {
		source = source; // kill compiler unused variable warning
		if (locations_.trace_indent_step) {
		    throw option_error("the 'trace-indent-step' option is only allowed once");
		}
		openum_ = option_trace_indent_step;
		locations_.trace_indent_step = position;
		state_ = state_value;
		return;
	    } else if (std::strcmp(option, "trace-main-file") == 0) {
		source = source; // kill compiler unused variable warning
		if (locations_.trace_main_file) {
		    throw option_error("the 'trace-main-file' option is only allowed once");
		}
		openum_ = option_trace_main_file;
		locations_.trace_main_file = position;
		state_ = state_value;
		return;
	    } else if (std::strcmp(option, "trace-max-width") == 0) {
		source = source; // kill compiler unused variable warning
		if (locations_.trace_max_width) {
		    throw option_error("the 'trace-max-width' option is only allowed once");
		}
		openum_ = option_trace_max_width;
		locations_.trace_max_width = position;
		state_ = state_value;
		return;
	    } else if (std::strcmp(option, "trace-min-width") == 0) {
		source = source; // kill compiler unused variable warning
		if (locations_.trace_min_width) {
		    throw option_error("the 'trace-min-width' option is only allowed once");
		}
		openum_ = option_trace_min_width;
		locations_.trace_min_width = position;
		state_ = state_value;
		return;
	    } else if (std::strcmp(option, "trace-space-characters") == 0) {
		source = source; // kill compiler unused variable warning
		if (locations_.trace_space_characters) {
		    throw option_error("the 'trace-space-characters' option is only allowed once");
		}
		openum_ = option_trace_space_characters;
		locations_.trace_space_characters = position;
		state_ = state_value;
		return;
	    } else if (std::strcmp(option, "trace-turn-off") == 0) {
		source = source; // kill compiler unused variable warning
		if (locations_.trace_turn_off) {
		    throw option_error("the 'trace-turn-off' option is only allowed once");
		}
		openum_ = option_trace_turn_off;
		locations_.trace_turn_off = position;
		state_ = state_value;
		return;
	    } else if (std::strcmp(option, "trace-turn-on") == 0) {
		source = source; // kill compiler unused variable warning
		if (locations_.trace_turn_on) {
		    throw option_error("the 'trace-turn-on' option is only allowed once");
		}
		openum_ = option_trace_turn_on;
		locations_.trace_turn_on = position;
		state_ = state_value;
		return;
	    } else if (std::strcmp(option, "trace-version") == 0) {
		source = source; // kill compiler unused variable warning
		if (locations_.trace_version) {
		    throw option_error("the 'trace-version' option is only allowed once");
		}
		openum_ = option_trace_version;
		locations_.trace_version = position;
		options_.trace_version = !options_.trace_version;
		return;
	    } else if (source == source_cl && std::strcmp(option, "help") == 0) {
		throw autoexcept(autothrow_help, const_usage);
	    }

    std::string error("unknown option '"); error += option; error += "'";
    throw option_error(error);
}
//#########################################################################
void DGD::parser::parse_value (const char *value) {
    switch (openum_) {
    	case option_trace_allow_word_wrap:
    	    {
    		bool bvalue;

    		if (std::strcmp(value, "true") == 0) {
    		    bvalue = true;
    		} else if (std::strcmp(value, "yes") == 0) {
    		    bvalue = true;
    		} else if (std::strcmp(value, "false") == 0) {
    		    bvalue = false;
    		} else if (std::strcmp(value, "no") == 0) {
    		    bvalue = false;
    		} else {
    		    std::string error("invalid boolean value '"); error += value; error += "'";
    		    throw option_error(error);
    		}
    		options_.trace_allow_word_wrap = bvalue;
    	    }
    	    break;
    	case option_trace_allow_wrap:
    	    {
    		bool bvalue;

    		if (std::strcmp(value, "true") == 0) {
    		    bvalue = true;
    		} else if (std::strcmp(value, "yes") == 0) {
    		    bvalue = true;
    		} else if (std::strcmp(value, "false") == 0) {
    		    bvalue = false;
    		} else if (std::strcmp(value, "no") == 0) {
    		    bvalue = false;
    		} else {
    		    std::string error("invalid boolean value '"); error += value; error += "'";
    		    throw option_error(error);
    		}
    		options_.trace_allow_wrap = bvalue;
    	    }
    	    break;
    	case option_trace_enable:
    	    break;
    	case option_trace_help:
    	    break;
    	case option_trace_indent_step:
    	    {
    		char *endptr; int tmp = std::strtol(value, &endptr, 0);
    		while (*endptr != 0 && std::isspace(*endptr)) ++endptr;

    		if (*endptr != 0) {
    		    std::string error("invalid integer value '"); error += value; error += "'";
    		    throw option_error(error);
    		}

    		options_.trace_indent_step = tmp;
    	    }
    	    break;
    	case option_trace_main_file:
    	    {
    		options_.trace_main_file = value;
    	    }
    	    break;
    	case option_trace_max_width:
    	    {
    		char *endptr; int tmp = std::strtol(value, &endptr, 0);
    		while (*endptr != 0 && std::isspace(*endptr)) ++endptr;

    		if (*endptr != 0) {
    		    std::string error("invalid integer value '"); error += value; error += "'";
    		    throw option_error(error);
    		}

    		options_.trace_max_width = tmp;
    	    }
    	    break;
    	case option_trace_min_width:
    	    {
    		char *endptr; int tmp = std::strtol(value, &endptr, 0);
    		while (*endptr != 0 && std::isspace(*endptr)) ++endptr;

    		if (*endptr != 0) {
    		    std::string error("invalid integer value '"); error += value; error += "'";
    		    throw option_error(error);
    		}

    		options_.trace_min_width = tmp;
    	    }
    	    break;
    	case option_trace_space_characters:
    	    {
    		options_.trace_space_characters = value;
    	    }
    	    break;
    	case option_trace_turn_off:
    	    {
    		options_.trace_turn_off = value;
    	    }
    	    break;
    	case option_trace_turn_on:
    	    {
    		options_.trace_turn_on = value;
    	    }
    	    break;
    	case option_trace_version:
    	    break;
    }
}
//#########################################################################
const char* DGD::option_error::get_help_comment (void) const {
    return const_help_comment;
}
//#########################################################################
namespace {
    const char* expand_long_name (const std::string &name) {
	std::string::size_type name_size = name.size();
	std::vector<const char*> matches;

        if (name_size <= 21 && name.compare(0, name_size, "trace-allow-word-wrap", name_size) == 0)
        	matches.push_back("trace-allow-word-wrap");

        if (name_size <= 16 && name.compare(0, name_size, "trace-allow-wrap", name_size) == 0)
        	matches.push_back("trace-allow-wrap");

        if (name_size <= 12 && name.compare(0, name_size, "trace-enable", name_size) == 0)
        	matches.push_back("trace-enable");

        if (name_size <= 10 && name.compare(0, name_size, "trace-help", name_size) == 0)
        	matches.push_back("trace-help");

        if (name_size <= 17 && name.compare(0, name_size, "trace-indent-step", name_size) == 0)
        	matches.push_back("trace-indent-step");

        if (name_size <= 15 && name.compare(0, name_size, "trace-main-file", name_size) == 0)
        	matches.push_back("trace-main-file");

        if (name_size <= 15 && name.compare(0, name_size, "trace-max-width", name_size) == 0)
        	matches.push_back("trace-max-width");

        if (name_size <= 15 && name.compare(0, name_size, "trace-min-width", name_size) == 0)
        	matches.push_back("trace-min-width");

        if (name_size <= 22 && name.compare(0, name_size, "trace-space-characters", name_size) == 0)
        	matches.push_back("trace-space-characters");

        if (name_size <= 14 && name.compare(0, name_size, "trace-turn-off", name_size) == 0)
        	matches.push_back("trace-turn-off");

        if (name_size <= 13 && name.compare(0, name_size, "trace-turn-on", name_size) == 0)
        	matches.push_back("trace-turn-on");

        if (name_size <= 13 && name.compare(0, name_size, "trace-version", name_size) == 0)
        	matches.push_back("trace-version");

        if (name_size <= 4 && name.compare(0, name_size, "help", name_size) == 0)
        	matches.push_back("help");


	if (matches.empty()) {
	    std::string error("unknown option '"); error += name; error += "'";
	    throw DGD::option_error(error);
	}

	if (matches.size() == 1) {
	    return matches[0];
	}

	std::string error("the option name '"); error += name; error += "' is ambiguous";
	throw DGD::option_error(error);
    }
} // end anonymous namespace