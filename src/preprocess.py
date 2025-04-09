import argparse
import random


def preprocess_graph(input_file, output_file):
    """
    Reads a graph file with edges in the format "u v w" (1-indexed nodes)
    and writes out the same edges with nodes zero-indexed.
    """
    with open(input_file, "r") as fin, open(output_file, "w") as fout:
        for line in fin:
            if not line.strip():
                continue
            parts = line.strip().split()
            if len(parts) != 3:
                continue
            u, v, w = parts
            try:
                u, v = int(u) - 1, int(v) - 1
                # Write zero-indexed edge to output file.
                fout.write(f"{u} {v} {w}\n")
            except ValueError:
                continue


def generate_queries_uniform(num_nodes, num_queries):
    """
    Generates a list of queries (source, target) with two distinct nodes,
    selected uniformly at random.
    """
    queries = []
    for _ in range(num_queries):
        u, v = random.sample(range(num_nodes), 2)
        queries.append((u, v))
    return queries


def generate_queries_with_self(num_nodes, num_queries):
    """
    Generates a list of queries (source, target) where picking the same node
    is allowed. Both source and target are chosen uniformly at random.
    """
    queries = []
    for _ in range(num_queries):
        u = random.randint(0, num_nodes - 1)
        v = random.randint(0, num_nodes - 1)
        queries.append((u, v))
    return queries


def generate_all_queries(num_nodes):
    """
    Generates all possible queries (source, target) for a graph with num_nodes.
    """
    queries = []
    for u in range(num_nodes):
        for v in range(num_nodes):
            queries.append((u, v))
    return queries


def write_queries_to_file(queries, output_file):
    """
    Writes the list of queries to a file, one query per line in the format "u v".
    """
    with open(output_file, "w") as fout:
        for u, v in queries:
            fout.write(f"{u} {v}\n")


def main():
    parser = argparse.ArgumentParser(
        description="Graph Preprocessing and Query Generation"
    )
    subparsers = parser.add_subparsers(dest="command", help="Sub-command help")

    # Subparser for preprocessing graph edges
    parser_preprocess = subparsers.add_parser(
        "preprocess", help="Preprocess graph edges (1-indexed -> 0-indexed)"
    )
    parser_preprocess.add_argument("input", help="Input file containing graph edges")
    parser_preprocess.add_argument("output", help="Output file for processed edges")

    # Subparser for generating queries
    parser_generate = subparsers.add_parser(
        "generate", help="Generate random queries for the graph"
    )
    parser_generate.add_argument(
        "num_nodes", type=int, help="Total number of nodes in the graph"
    )
    parser_generate.add_argument("output", help="Output file for queries")

    parser_generate.add_argument(
        "--method",
        choices=["uniform", "with_self", "all"],
        default="uniform",
        help="Query generation method (default: uniform)",
    )
    parser_generate.add_argument(
        "--num_queries",
        type=int,
        default=0,
        help="Number of queries to generate (default: 0)",
    )

    args = parser.parse_args()

    if args.command == "preprocess":
        preprocess_graph(args.input, args.output)
        print(f"Preprocessed edges written to {args.output}")
    elif args.command == "generate":
        if args.method == "uniform":
            queries = generate_queries_uniform(args.num_nodes, args.num_queries)
        elif args.method == "all":
            queries = generate_all_queries(args.num_nodes)
        elif args.method == "with_self":
            queries = generate_queries_with_self(args.num_nodes, args.num_queries)
        write_queries_to_file(queries, args.output)
        print(
            f"Generated {len(queries)} queries using method '{args.method}' written to {args.output}"
        )
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
