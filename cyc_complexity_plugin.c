/*
 * Copyright 2012, 2013, 2014 by Emese Revfy <re.emese@gmail.com>
 * Licensed under the GPL v2, or (at your option) v3
 *
 * http://en.wikipedia.org/wiki/Cyclomatic_complexity
 * The complexity M is then defined as:
 * M = E − N + 2P
 * where
 *
 *  E = the number of edges of the graph
 *  N = the number of nodes of the graph
 *  P = the number of connected components (exit nodes).
 *
 * Homepage:
 * http://www.grsecurity.net/~ephox/
 *
 * Usage:
 * $ # for 4.5/4.6/C based 4.7
 * $ gcc -I`gcc -print-file-name=plugin`/include -I`gcc -print-file-name=plugin`/include/c-family -fPIC -shared -O2 -std=gnu99 -ggdb -o cyc_complexity_plugin.so cyc_complexity_plugin.c
 * $ # for C++ based 4.7/4.8+
 * $ g++ -I`g++ -print-file-name=plugin`/include -I`g++ -print-file-name=plugin`/include/c-family -fPIC -shared -O2 -std=gnu++98 -fno-rtti -ggdb -o cyc_complexity_plugin.so cyc_complexity_plugin.c
 *
 * $ gcc -fplugin=./cyc_complexity_plugin.so test.c -O2
 */

#include "gcc-common.h"

int plugin_is_GPL_compatible;

static struct plugin_info cyc_complexity_plugin_info = {
	.version	= "20140218",
	.help		= "Cyclomatic Complexity\n",
};

static unsigned int handle_function(void)
{
	int complexity;
	expanded_location xloc;

	// M = E − N + 2P
	complexity = n_edges_for_fn(cfun) - n_basic_blocks_for_fn(cfun) + 2;

	xloc = expand_location(DECL_SOURCE_LOCATION(current_function_decl));
	fprintf(stderr, "Cyclomatic Complexity %d %s:%s\n", complexity, xloc.file, DECL_NAME_POINTER(current_function_decl));
//	inform(loc, "Cyclomatic Complexity %d %s\n", complexity, DECL_NAME_POINTER(current_function_decl));

	return 0;
}

#if BUILDING_GCC_VERSION >= 4009
static const struct pass_data cyc_complexity_pass_data = {
#else
static struct gimple_opt_pass cyc_complexity_pass = {
	.pass = {
#endif
		.type			= GIMPLE_PASS,
		.name			= "cyc_complexity",
#if BUILDING_GCC_VERSION >= 4008
		.optinfo_flags		= OPTGROUP_NONE,
#endif
#if BUILDING_GCC_VERSION >= 4009
		.has_gate		= false,
		.has_execute		= true,
#else
		.gate			= NULL,
		.execute		= handle_function,
		.sub			= NULL,
		.next			= NULL,
		.static_pass_number	= 0,
#endif
		.tv_id			= TV_NONE,
		.properties_required	= 0,
		.properties_provided	= 0,
		.properties_destroyed	= 0,
		.todo_flags_start	= 0,
		.todo_flags_finish	= TODO_dump_func
#if BUILDING_GCC_VERSION < 4009
	}
#endif
};

#if BUILDING_GCC_VERSION >= 4009
namespace {
class cyc_complexity_pass : public gimple_opt_pass {
public:
	cyc_complexity_pass() : gimple_opt_pass(cyc_complexity_pass_data, g) {}
	unsigned int execute() { return handle_function(); }
};
}
#endif

static struct opt_pass *make_cyc_complexity_pass(void)
{
#if BUILDING_GCC_VERSION >= 4009
	return new cyc_complexity_pass();
#else
	return &cyc_complexity_pass.pass;
#endif
}

int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
	const char * const plugin_name = plugin_info->base_name;
	struct register_pass_info cyc_complexity_pass_info;

	cyc_complexity_pass_info.pass				= make_cyc_complexity_pass();
	cyc_complexity_pass_info.reference_pass_name		= "ssa";
	cyc_complexity_pass_info.ref_pass_instance_number	= 1;
	cyc_complexity_pass_info.pos_op				= PASS_POS_INSERT_AFTER;

	if (!plugin_default_version_check(version, &gcc_version)) {
		error(G_("incompatible gcc/plugin versions"));
		return 1;
	}

	register_callback(plugin_name, PLUGIN_INFO, NULL, &cyc_complexity_plugin_info);
	register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &cyc_complexity_pass_info);

	return 0;
}
