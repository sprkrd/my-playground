import numpy as np

from scipy.linalg import expm


class Graph:

    def __init__(self, *features, node_labels=None, edge_labels=None):
        nodes = set()

        node_labels_are_given = node_labels is not None
        edge_labels_are_given = edge_labels is not None

        node_labels = node_labels or set()
        edge_labels = edge_labels or set()

        r_v = {}
        r_e = {}

        for f in features:
            for u in f[1:]:
                nodes.add(u)
            if len(f) == 2: # node feature
                label, u = f
                if node_labels_are_given:
                    assert label in node_labels
                else:
                    node_labels.add(label)
                r_v.setdefault(u, []).append(label)
            elif len(f) == 3: # edge feature
                label, u, v = f
                if edge_labels_are_given:
                    assert label in edge_labels
                else:
                    edge_labels.add(label)
                r_e.setdefault(u, []).append((label, v))

        if not isinstance(nodes, list):
            nodes = list(nodes)
            nodes.sort()
        if not isinstance(node_labels, list):
            node_labels = list(node_labels)
        if not isinstance(edge_labels, list):
            edge_labels = list(edge_labels)

        self.nodes = nodes
        self.node_labels = node_labels
        self.edge_labels = edge_labels
        self.r_v = r_v
        self.r_e = r_e


    def label_matrix(self):
        n_labels = len(self.node_labels)
        n_nodes = len(self.nodes)
        label_matrix = np.zeros((n_labels, n_nodes))
        for u, labels in self.r_v.items():
            u_idx = self.nodes.index(u)
            for l in labels:
                l_idx = self.node_labels.index(l)
                label_matrix[l_idx,u_idx] = 1
        return label_matrix

    def adjacency_matrix(self):
        n_labels = len(self.edge_labels)
        n_nodes = len(self.nodes)
        adj_matrix = np.zeros((n_labels, n_nodes, n_nodes))
        for u, adj_list in self.r_e.items():
            u_idx = self.nodes.index(u)
            for l, v in adj_list:
                l_idx = self.edge_labels.index(l)
                v_idx = self.nodes.index(v)
                adj_matrix[l_idx,u_idx,v_idx] = 1
        return adj_matrix

    def exponential_kernel(self):
        n_node_labels = len(self.node_labels)
        n_edge_labels = len(self.edge_labels)
        label_matrix = self.label_matrix()
        adj_mat = self.adjacency_matrix()
        kernel = np.zeros((n_edge_labels,n_node_labels,n_node_labels))
        for idx,plane in enumerate(adj_mat):
            running_product = np.matmul(label_matrix, expm(plane))
            np.matmul(running_product, label_matrix.T, kernel[idx])
        return kernel



def main():
    # g = Graph(("clear", "disc0"), ("on", "disc0", "disc1"), ("on", "disc1", "peg0"),
            # ("clear", "peg1"), ("clear", "peg2"), ("smaller", "disc0", "disc1"),
            # ("smaller", "disc0", "peg0"), ("smaller", "disc0", "peg1"),
            # ("smaller", "disc0", "peg2"), ("smaller", "disc1", "peg0"),
            # ("smaller", "disc1", "peg1"), ("smaller", "disc1", "peg2"),
            # ("goal_on", "disc0", "disc1"), ("goal_on", "disc1", "peg0"),
            # ("disc", "disc0"), ("disc", "disc1"), ("peg", "peg0"),
            # ("peg", "peg1"), ("peg", "peg2"),
            # edge_labels=["goal_on", "on", "smaller"],
            # node_labels=["disc", "peg", "clear"])
    # g = Graph(("clear", "disc0"), ("on", "disc0", "disc1"), ("on", "disc1", "peg1"),
            # ("clear", "peg0"), ("clear", "peg2"), ("smaller", "disc0", "disc1"),
            # ("smaller", "disc0", "peg0"), ("smaller", "disc0", "peg1"),
            # ("smaller", "disc0", "peg2"), ("smaller", "disc1", "peg0"),
            # ("smaller", "disc1", "peg1"), ("smaller", "disc1", "peg2"),
            # ("goal_on", "disc0", "disc1"), ("goal_on", "disc1", "peg2"),
            # ("disc", "disc0"), ("disc", "disc1"), ("peg", "peg0"),
            # ("peg", "peg1"), ("peg", "peg2"),
            # edge_labels=["goal_on", "on", "smaller"],
            # node_labels=["disc", "peg", "clear"])
    g = Graph(("clear", "disc0"), ("clear", "disc1"), ("on", "disc0", "peg1"),
            ("on", "disc1", "peg0"), ("clear", "peg2"), ("smaller", "disc0", "disc1"),
            ("smaller", "disc0", "peg0"), ("smaller", "disc0", "peg1"),
            ("smaller", "disc0", "peg2"), ("smaller", "disc1", "peg0"),
            ("smaller", "disc1", "peg1"), ("smaller", "disc1", "peg2"),
            ("goal_on", "disc0", "disc1"), ("goal_on", "disc1", "peg0"),
            ("disc", "disc0"), ("disc", "disc1"), ("peg", "peg0"),
            ("peg", "peg1"), ("peg", "peg2"),
            edge_labels=["goal_on", "on", "smaller"],
            node_labels=["disc", "peg", "clear"])
    print(g.nodes)
    print(g.node_labels)
    print(g.label_matrix())
    print(g.edge_labels)
    adj_mat = g.adjacency_matrix()
    print(adj_mat)
    # print(adj_mat[0])
    print(g.exponential_kernel())
    # print(g.adjacency_matrix())


if __name__ == "__main__":
    main()


