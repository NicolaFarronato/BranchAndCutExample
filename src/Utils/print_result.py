import networkx as nx
import matplotlib.pyplot as plt
import sys


def print_graph(src: str):
    g = nx.read_edgelist(src,
                         nodetype=int,
                         data=(('weight', float),), create_using=nx.Graph())

    pos = nx.spring_layout(g)
    nx.draw_networkx_nodes(g, pos)
    nx.draw_networkx_labels(g, pos)
    nx.draw_networkx_edges(g, pos)
    nx.draw_networkx_edges(g, pos)
    plt.savefig("graph.png", format="PNG")


def print_prova():
    print("Ciao")


if __name__ == '__main__':
    arg = sys.argv[1]
    print_graph(arg)
