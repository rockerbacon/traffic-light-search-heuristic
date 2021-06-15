#include <cpp-command-line-interface/command_line_interface.h>
#include "traffic_graph/traffic_graph.h"

#include <fstream>

#define DEFAULT_MIN_VERTEX_DEGREE 4
#define DEFAULT_MAX_VERTEX_DEGREE 10
#define DEFAULT_MIN_EDGE_WEIGHT 1
#define UNSPECIFIED_WEIGHT -1

using namespace traffic;
using namespace std;

cli_main (
	"Random Graph Generator",
	"v0.1",
	"Program for creating random graphs",

	cli::RequiredArgument<string> output_file_path("output");
	cli::create_alias("output", 'o');

	cli::RequiredArgument<Vertex> number_of_vertices("vertices");
	cli::create_alias("vertices", 'v');

	cli::RequiredArgument<TimeUnit> cycle("cycle");
	cli::create_alias("cycle", 'c');

	cli::OptionalArgument<Vertex> min_vertex_degree(DEFAULT_MIN_VERTEX_DEGREE, "min-vertex-degree");
	cli::create_alias("min-vertex-degree", 'm');

	cli::OptionalArgument<Vertex> max_vertex_degree(DEFAULT_MAX_VERTEX_DEGREE, "max-vertex-degree");
	cli::create_alias("max-vertex-degree", 'M');

	cli::OptionalArgument<TimeUnit> min_edge_weight(DEFAULT_MIN_EDGE_WEIGHT, "min-edge-weight");
	cli::create_alias("min-edge-weight", 'w');

	cli::OptionalArgument<TimeUnit> max_edge_weight(UNSPECIFIED_WEIGHT, "max-edge-weight");
	cli::create_alias("max-edge-weight", 'W');
) {

	GraphBuilder *builder;
	ofstream file_stream;

	if (*max_edge_weight == UNSPECIFIED_WEIGHT) {
		*max_edge_weight = *cycle-1;
	}

	builder = new GraphBuilder(*number_of_vertices, *min_vertex_degree, *max_vertex_degree, *min_edge_weight, *max_edge_weight);
	builder->withCycle(*cycle);

	file_stream.open(*output_file_path);
	builder->output_to_file(file_stream);
	file_stream.close();

	delete builder;

	// debugging
#ifdef DEBUG
	GraphBuilder *debug_builder;
	ifstream debug_stream;

	debug_builder = new GraphBuilder();

	debug_stream.open(*output_file_path);
	debug_builder->read_from_file(debug_stream);
	debug_stream.close();

	file_stream.open(*output_file_path+"_debug");
	debug_builder->output_to_file(file_stream);
	file_stream.close();

	delete debug_builder;
#endif


} end_cli_main;
