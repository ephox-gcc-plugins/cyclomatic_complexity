/*
 * Copyright 2011-2016 by Emese Revfy <re.emese@gmail.com>
 * Licensed under the GPL v2, or (at your option) v3
 *
 * Homepage:
 * https://github.com/ephox-gcc-plugins/cyclomatic_complexity
 *
 * http://en.wikipedia.org/wiki/Cyclomatic_complexity
 * The complexity M is then defined as:
 * M = E - N + 2P
 * where
 *
 *  E = the number of edges of the graph
 *  N = the number of nodes of the graph
 *  P = the number of connected components (exit nodes).
 *
 * Usage (4.5 - 5):
 * $ make clean; make run
 */

#include "gcc-common.h"

int plugin_is_GPL_compatible;

static struct plugin_info cyc_complexity_plugin_info = {
	.version	= "20160202",
	.help		= "Cyclomatic Complexity\n",
};

static unsigned int handle_function(void)
{
	int complexity;
	expanded_location xloc;

	// M = E - N + 2P
	complexity = n_edges_for_fn(cfun) - n_basic_blocks_for_fn(cfun) + 2;

	xloc = expand_location(DECL_SOURCE_LOCATION(current_function_decl));
	fprintf(stderr, "Cyclomatic Complexity %d %s:%s\n", complexity, xloc.file, DECL_NAME_POINTER(current_function_decl));

	return 0;
}

#if BUILDING_GCC_VERSION >= 4009
namespace {
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
#if BUILDING_GCC_VERSION >= 5000
#elif BUILDING_GCC_VERSION >= 4009
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
class cyc_complexity_pass : public gimple_opt_pass {
public:
	cyc_complexity_pass() : gimple_opt_pass(cyc_complexity_pass_data, g) {}
#if BUILDING_GCC_VERSION >= 5000
	virtual unsigned int execute(function *) { return handle_function(); }
#else
	unsigned int execute() { return handle_function(); }
#endif
};
}

static struct opt_pass *make_cyc_complexity_pass(void)
{
	return new cyc_complexity_pass();
}
#else
static struct opt_pass *make_cyc_complexity_pass(void)
{
	return &cyc_complexity_pass.pass;
}
#endif

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
