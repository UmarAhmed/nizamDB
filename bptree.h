#ifndef BPTREE_H
#define BPTREE_H


#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>


// base Node class
struct Node {
    std::shared_ptr<Node> parent;
    bool is_leaf;
    virtual ~Node() = 0;
};

Node::~Node() {}


// Invariant: keys.size() == children.size() - 1
template <typename K>
struct InternalNode: public Node {

    std::vector<K> keys;
    std::vector< std::shared_ptr<Node> > children;

    // Insert key and child in sorted order
    void insert(const K k, std::shared_ptr<Node > child) {
        // If we split root, then the new root's keys will be empty
        if (keys.empty()) {
            keys.push_back(k);
            children.push_back(child);
            return;
        }
        auto itr = std::lower_bound(keys.begin(), keys.end(), k);
        const int index = itr - keys.begin();
        keys.insert(itr, k); 
        if (index == 0) {
            children.insert(children.begin(), child);
        } else {
            children.insert(children.begin() + index + 1, child);
        }
    }
};


template <typename K, typename D>
struct LeafNode: public Node {

    std::shared_ptr<LeafNode> next;
    std::vector<K> keys;
    std::vector<D> data; // TODO change to pointers later

    // Insert key and data in sorted order
    void insert(const K k, const D d) {
        auto itr = std::lower_bound(keys.begin(), keys.end(), k);
        const int index = itr - keys.begin();

        keys.insert(itr, k);
        data.insert(data.begin() + index, d);
    }
};


template <typename K, typename D>
class BPTree {
    std::shared_ptr<Node> root;
    const int b;
    int _size;


    // only called when splitting root
    // NOTE: creates InternalNode as parent which contains 1 child (0 keys)
    void deorphan(std::shared_ptr<Node> current) {
        // If no parent, then create a new parent
        if (current->parent == nullptr) {
            auto new_parent = std::make_shared<InternalNode<K> >();
            new_parent->is_leaf = false;
            new_parent->parent = nullptr;
            new_parent->children.push_back(current);

            current->parent = new_parent;
            root = new_parent;

        }
    }

    // splits internal node
    void split_internal(std::shared_ptr<InternalNode<K>> current) {
        // Ensure parent exists
        deorphan(current);

        // Create sibling node
        auto new_node = std::make_shared<InternalNode<K> >();
        new_node->is_leaf = false;
        new_node->parent = current->parent;

        // Move middle key into parent
        const int mid = current->keys.size() / 2;
        auto parent = std::dynamic_pointer_cast<InternalNode<K>>(current->parent);
        parent->insert(current->keys[mid], new_node);

        // Split the other keys and children
        for (int i = mid + 1; i < current->keys.size(); ++i) {
            new_node->keys.emplace_back(std::move(current->keys[i]));
            new_node->children.emplace_back(std::move(current->children[i]));
            new_node->children.back()->parent = new_node;
        }
        new_node->children.emplace_back(std::move(current->children.back() ));
        new_node->children.back()->parent = new_node;

        // Remove the items we copied
        current->keys.resize(mid);
        current->children.resize(mid + 1);

        // Handle splitting of ancestors if necessary
        if (parent->keys.size() == b) {
            split_internal(parent);
        }
    }

    // splits leaf node
    void split_leaf(std::shared_ptr<LeafNode<K, D>> leaf) {
        // Ensure a parent exists
        deorphan(leaf);

        // Create sibling node
        auto new_node = std::make_shared<LeafNode<K, D> >();
        new_node->is_leaf = true;
        new_node->parent = leaf->parent;
        new_node->next = leaf->next;
        leaf->next = new_node;

        // Copy half of the keys and data
        const int mid = leaf->keys.size() / 2;
        for (int i = mid; i < leaf->keys.size(); ++i) {
            new_node->keys.emplace_back(std::move(leaf->keys[i]));
            new_node->data.emplace_back(std::move(leaf->data[i]));
        }

        // Remove the items we copied
        leaf->keys.resize(mid);
        leaf->data.resize(mid);

        // Add key and child for new_node to parent, and split if necessary
        auto parent = std::dynamic_pointer_cast<InternalNode<K>>(leaf->parent);
        parent->insert(new_node->keys[0], new_node);

        if (parent->keys.size() == b) {
            split_internal(parent);
        }
    }

    void print_node(std::shared_ptr<Node> current) const {
        if (current->is_leaf) {
            auto temp = std::dynamic_pointer_cast<LeafNode<K, D>>(current);
            for (auto k: temp->keys) {
                std::cout << k << ", ";
            }
        } else {
            auto temp = std::dynamic_pointer_cast<InternalNode<K>>( current);
            for (auto k: temp->keys) {
                std::cout << k << ", ";
            }
        }

        std::cout << std::endl;
    }

    void print(std::shared_ptr<Node> current) const {
        print_node(current);
        if (!current->is_leaf) {
            auto temp = std::dynamic_pointer_cast<InternalNode<K>>(current);
            for (auto child: temp->children) {
                print(child);
            }
        }
    }

    std::shared_ptr<Node> find_node(const K& k) const {
        if (root == nullptr) {
            return nullptr;
        }

        // Otherwise search for the right node
        auto current = root;

        while (!current->is_leaf) {
            auto temp = std::dynamic_pointer_cast<InternalNode<K>>(current);
            int i = 0;
            for (const auto key: temp->keys) {
                if (k < key) {
                    break;
                }
                ++i;
            }
            current = temp->children[i];
        }

        return current;
    }

public:
    // Default constructor
    BPTree(): b {4} {
        root = nullptr;
        _size = 0;
    }

    BPTree(const int _b): b {_b} {
        root = nullptr;
        _size = 0;
    }

    int size() const {
        return _size;
    }

    std::shared_ptr<D> search(const K& k) const {
        // Find the containing leaf node
        auto node = find_node(k);

        if (node == nullptr) {
            return nullptr;
        }

        // Search the node
        auto leaf = std::dynamic_pointer_cast<LeafNode<K, D>>(node);
        for (int i = 0; i < leaf->keys.size(); ++i) {
            if (k == leaf->keys[i]) {
                // TODO fix later
                auto temp = std::make_shared<D>(leaf->data[i]);
                return temp;
            }
        }

        return nullptr;
    }

    // maybe we could pass a compare operator that would allow a key to be compared
    // to low and upper (so we could do < and <= on both upper and lower)
    // or could pass two bools
    // TODO generalize this
    std::vector<D> range_query(const K low, const K& hi) {
        if (hi < low) {
            return {};
        }
        auto leaf = std::dynamic_pointer_cast<LeafNode<K, D>>(find_node(low));
        std::vector<D> result;

        while (leaf) {
            // Find keys in range, and save data to result
            auto i = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), low);
            auto j = std::upper_bound(leaf->keys.begin(), leaf->keys.end(), hi);

            const auto data_i = leaf->data.begin() + (i - leaf->keys.begin());
            const auto data_j = leaf->data.begin() + (j - leaf->keys.begin());

            result.insert(result.end(), data_i, data_j);

            if (j != leaf->keys.end()) {
                break;
            }

            leaf = leaf->next;
        }
        return result;
    }


    // Insert a record
    void insert(const K& k, const D& d) {
        // Create new node if needed
        if (root == nullptr) {
            root = std::make_shared <LeafNode<K, D>>();
            auto temp = std::dynamic_pointer_cast<LeafNode<K, D>>(root);
            
            temp->is_leaf = true;
            temp->next = nullptr;
            temp->parent = nullptr;
            temp->keys.push_back(k);
            temp->data.push_back(d);


            ++_size;
            return;
        }

        // Otherwise search for the right node
        auto current = find_node(k);

        // Insert into node and split if necessary
        auto temp = std::dynamic_pointer_cast<LeafNode<K, D>>(current);
        temp->insert(k, d);
        ++_size;

        // Perform splitting procedure if node is full
        if (temp->keys.size() == b) {
            split_leaf(temp);
        }
    }

    void print() const {
        std::cout << "---\n";
        print(root);
        std::cout << "---\n";
    }
};

#endif
