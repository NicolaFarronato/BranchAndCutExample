import networkx as nx
import matplotlib.pyplot as plt
import sys
from CVRPUtils import CVRPUtils


def print_graph(src: str):
    cv = CVRPUtils(src)
    cv.plot_graph_cvrp()
    plt.savefig("graph.png", format="PNG")


if __name__ == '__main__':
    arg = sys.argv[1]
    print_graph(arg)
