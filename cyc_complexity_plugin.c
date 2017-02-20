/*
 * Copyright 2011-2017 by Emese Revfy <re.emese@gmail.com>
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
 * Options:
 * -fplugin-arg-cyc_complexity_plugin-log_file
 *
 * Usage (4.5 - 6):
 * $ make clean; make run
 */

#include "gcc-common.h"

__visible int plugin_is_GPL_compatible;

bool has_log_file;
FILE *log_file;

static struct plugin_info cyc_complexity_plugin_info = {
	.version	= "20170220",
	.help		= "Cyclomatic Complexity\n",
};

static unsigned int cyc_complexity_execute(void)
{
	int complexity;
	expanded_location xloc;

	/* M = E - N + 2P */
	complexity = n_edges_for_fn(cfun) - n_basic_blocks_for_fn(cfun) + 2;

	xloc = expand_location(DECL_SOURCE_LOCATION(current_function_decl));
	fprintf(log_file, "Cyclomatic Complexity %d %s:%s\n", complexity,
		xloc.file, DECL_NAME_POINTER(current_function_decl));

	return 0;
}

static void cyc_complexity_start_unit(void __unused *gcc_data, void __unused *user_data)
{
	char *filename;

	if (!has_log_file) {
		log_file = stderr;
		return;
	}

	filename = concat(aux_base_name, ".", "cyc_complexity", NULL);
	log_file = fopen(filename, "w");
	if (!log_file)
		error("Can't open %s for writing.", filename);
	free(filename);
}

static void cyc_complexity_finish_unit(void __unused *gcc_data, void __unused *user_data)
{
	if (has_log_file)
		fclose(log_file);
}

#define PASS_NAME cyc_complexity

#define NO_GATE
#define TODO_FLAGS_FINISH TODO_dump_func

#include "gcc-generate-gimple-pass.h"

__visible int plugin_init(struct plugin_name_args *plugin_info, struct plugin_gcc_version *version)
{
	int i;
	const char * const plugin_name = plugin_info->base_name;
	const int argc = plugin_info->argc;
	const struct plugin_argument * const argv = plugin_info->argv;

	PASS_INFO(cyc_complexity, "ssa", 1, PASS_POS_INSERT_AFTER);

	if (!plugin_default_version_check(version, &gcc_version)) {
		error_gcc_version(version);
		return 1;
	}

	for (i = 0; i < argc; ++i) {
		if (!strcmp(argv[i].key, "log_file")) {
			has_log_file = true;
			continue;
		}

		error(G_("unknown option '-fplugin-arg-%s-%s'"), plugin_name, argv[i].key);
	}

	register_callback(plugin_name, PLUGIN_START_UNIT,
				&cyc_complexity_start_unit, NULL);
	register_callback (plugin_name, PLUGIN_FINISH_UNIT,
				&cyc_complexity_finish_unit, NULL);
	register_callback(plugin_name, PLUGIN_INFO, NULL,
				&cyc_complexity_plugin_info);
	register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, NULL,
				&cyc_complexity_pass_info);

	return 0;
}
