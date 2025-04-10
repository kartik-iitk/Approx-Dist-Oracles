import argparse
import random
import os


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
    parser.add_argument("-t", type=str, required=True, help="Test Case Folder")
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

    parameters_file = args.t + "/inp/parameters.txt"
    parameters = {}
    with open(parameters_file, "r") as file:
        for i, line in enumerate(file, start=1):
            value = int(line.strip())
            if i == 1:
                parameters["n"] = value
            elif i == 2:
                parameters["k"] = value
            elif i == 3:
                parameters["num_threads"] = value
            elif i == 4:
                parameters["g"] = value
            elif i == 5:
                parameters["both"] = True if value == 1 else False
            elif i == 6:
                parameters["spaceopt"] = True if value == 1 else False
            elif i == 7:
                parameters["verbose"] = True if value == 1 else False
            elif i == 8:
                parameters["debug"] = True if value == 1 else False

    is_one_indexed = True if parameters["g"] == 1 else False

    queries = []
    if args.m == "uniform":
        queries = generate_queries_uniform(parameters["n"], args.q, is_one_indexed)
    elif args.m == "all":
        queries = generate_all_queries(parameters["n"], is_one_indexed)
    elif args.m == "with_self":
        queries = generate_queries_with_self(parameters["n"], args.q, is_one_indexed)

    outputfile = args.t + "/inp/queries.txt"
    if os.path.exists(outputfile):
        os.remove(outputfile)

    write_queries_to_file(queries, outputfile)
    print(
        f"Generated {len(queries)} queries using method '{args.m}' with indexing '{1 if is_one_indexed else 0}' written to {outputfile}"
    )


if __name__ == "__main__":
    main()
