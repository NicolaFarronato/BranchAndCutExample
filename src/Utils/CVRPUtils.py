import os

import networkx as nx
import numpy as np
import matplotlib.pyplot as plt

import sys


class CVRPUtils:

    def __init__(self, path: str):
        self._ratio_xy = None
        self.g = None
        self.dem = None
        self.d = None
        self.get_graph(path)

    def get_graph(self, path: str):
        coords = np.loadtxt(os.path.join(path, "coords.txt"), dtype=int)
        c = []
        self.dem = []  # demand
        has_coord = False
        x_points = []
        y_points = []
        for n, point in enumerate(coords):
            if point.size > 1:
                c.append((point[0], point[1]))
                x_points.append(point[0])
                y_points.append(point[1])
                self.dem.append(point[2])
                has_coord = True
            else:
                self.dem.append(point)
        edges = np.loadtxt(os.path.join(path,"solutionEdges.txt"), dtype=int)
        e = []
        for edge in edges:
            e.append((edge[0], edge[1], edge[2]))

        def add_edge_to_graph(g, e1, e2, w):
            g.add_edge(e1, e2, weight=w)

        self.d = dict()
        for i, ed in enumerate(c):
            self.d[i] = ed



        self.g = nx.Graph()
        for edge in e:
            add_edge_to_graph(self.g, edge[0], edge[1], edge[2])

        for node, pos in self.d.items():  # Add Node positions
            self.g.nodes[node]['pos'] = pos

        if x_points:
            range_x = max(x_points) - min(x_points)
            range_y = max(y_points) - min(y_points)
            self._ratio_xy = range_x / range_y

        if not x_points:
            self.d = nx.spring_layout(self.g)

    def plot_graph_cvrp(self, save_path=""):
        ret = nx.cycle_basis(self.g, root=0)
        objective_value = 0

        n = len(self.g.nodes())
        seq = [*range(n)]
        cycle_colors = [0.0] * len(self.g.edges)
        cycle_line_type = ['solid'] * len(self.g.edges)
        curr_color_val = 0
        edge_list = list(self.g.edges)

        for n, c in enumerate(ret):
            curr_obj_val = 0
            curr_dem = 0
            prev = 0
            for curr in c:
                if curr in seq: seq.remove(curr)
                cw = self.g.edges[prev, curr]['weight']
                try:
                    index = edge_list.index((prev, curr))
                except:
                    index = edge_list.index((curr, prev))
                cycle_colors[index] = curr_color_val

                if prev == 0 or curr == 0:
                    cycle_line_type[index] = 'dotted'
                curr_obj_val += cw
                prev = curr
                curr_dem += self.dem[curr]
            curr_color_val += 1.0
            objective_value += curr_obj_val
            print("Cycle {}, nodes 0 {}, curr obj {}, cycle demand {} ".format(n + 1, " ".join(str(x) for x in c),
                                                                               curr_obj_val,
                                                                               curr_dem))
        if seq:
            for s in seq:
                n += 1
                cw = self.g.edges[0, s]['weight']
                index = edge_list.index((0, s))
                cycle_colors[index] = curr_color_val
                cycle_line_type[index] = 'dotted'
                objective_value += cw * 2
                print("Route {}, nodes 0 {} 0, curr obj {}, cycle demand {} ".format(n + 1, s, cw * 2, self.dem[s]))

        print("Total objective value {}".format(objective_value))

        ## Plot ##
        d_labels = dict()
        for k, v in self.d.items():
            d_labels[k] = (v[0] + 1, v[1] + 1)

        # Color map and labels
        cmap_name = "tab20" if len(cycle_colors) < 21 else "hsv"
        plot_labels = True if len(self.g.edges) < 50 else False

        # PlotSize

        base_size = 6 if len(self.g.nodes) < 50 else 6 + ((len(self.g.nodes)) / 25.0)

        plt.figure(figsize=(base_size * self._ratio_xy, base_size * self._ratio_xy))
        nx.draw_networkx(self.g, pos=self.d, with_labels=plot_labels, edge_color=cycle_colors,
                         edge_cmap=plt.colormaps[cmap_name], node_color='black', node_size=9,
                         font_size=10.5, style=cycle_line_type)
        if save_path != "":
            plt.savefig(os.path.join(save_path, "graph.png"), format="PNG")

    def plot_graph_simple(self, save_path=""):
        nx.draw_networkx(self.g, pos=self.d, with_labels=True)
        if save_path != "":
            plt.savefig(os.path.join(save_path, "graph.png"), format="PNG")
