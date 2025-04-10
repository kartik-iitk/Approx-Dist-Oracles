import argparse
import random


def generate_queries_uniform(num_nodes, num_queries, is_one_indexed):
    """
    Generates a list of queries (source, target) with two distinct nodes,
    selected uniformly at random.
    """
    offset = 1 if is_one_indexed else 0
    queries = []
    for _ in range(num_queries):
        u, v = random.sample(range(offset, num_nodes + offset), 2)
        queries.append((u, v))
    return queries


def generate_queries_with_self(num_nodes, num_queries, is_one_indexed):
    """
    Generates a list of queries (source, target) where picking the same node
    is allowed. Both source and target are chosen uniformly at random.
    """
    offset = 1 if is_one_indexed else 0
    queries = []
    for _ in range(num_queries):
        u = random.randint(offset, num_nodes - 1 + offset)
        v = random.randint(offset, num_nodes - 1 + offset)
        queries.append((u, v))
    return queries


def generate_all_queries(num_nodes, is_one_indexed):
    """
    Generates all possible queries (source, target) for an undirected graph with num_nodes.
    Ensures that (u, v) and (v, u) are not both included and avoids self-loops.
    """
    offset = 1 if is_one_indexed else 0
    queries = []
    for u in range(offset, num_nodes + offset):
        for v in range(u, num_nodes + offset):
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
    parser = argparse.ArgumentParser(description="Query Generation")
    parser.add_argument("-n", type=int, help="Total number of nodes in the graph")
    parser.add_argument(
        "-i",
        type=int,
        choices=[0, 1],
        help="Specify whether the graph is zero-indexed or one-indexed (default: zero)",
    )
    parser.add_argument("-o", type=str, help="Output file for queries")
    parser.add_argument(
        "-m",
        choices=["uniform", "with_self", "all"],
        default="all",
        help="Query generation method (default: all)",
    )
    parser.add_argument(
        "-q",
        type=int,
        default=10,
        help="Number of queries to generate (default: 10)",
    )

    args = parser.parse_args()

    is_one_indexed = args.i == 1

    queries = []
    if args.m == "uniform":
        queries = generate_queries_uniform(args.n, args.q, is_one_indexed)
    elif args.m == "all":
        queries = generate_all_queries(args.n, is_one_indexed)
    elif args.m == "with_self":
        queries = generate_queries_with_self(args.n, args.q, is_one_indexed)

    write_queries_to_file(queries, args.o)
    print(
        f"Generated {len(queries)} queries using method '{args.m}' with indexing '{args.i}' written to {args.o}"
    )


if __name__ == "__main__":
    main()
