#include <memory>
#include <vector>
#include <algorithm>


// base Node class
template <typename K, typename D>
struct Node {
    virtual ~Node() = 0; // pure virtual dtor
};



// Invariant: keys.size() == children.size() - 1
template <typename K, typename D>
struct InternalNode: public Node<K, D> {
    
    std::unique_ptr<InternalNode<K,D> > parent;

    std::vector<K>  keys;
    std::vector< std::unique_ptr<Node<K, D> >> children;

    bool is_leaf;

    // Insert key and child in sorted order
    void insert(const K k, std::unique_ptr<Node<K, D>> child) {

        
        auto itr = std::lower_bound(keys.begin(), keys.end(), k);
        const int index = itr - keys.begin();
        keys.insert(itr, k); 
    }
};


// LeafNode class
template <typename K, typename D>
struct LeafNode: public Node<K, D> {

    std::unique_ptr<LeafNode> next;
    std::unique_ptr<InternalNode<K,D> > parent;
    
    std::vector<K>  keys;
    std::vector< std::unique_ptr<D>> data;

    bool is_leaf;

    // Insert key and data in sorted order
    void insert(const K k, const D d) {
        auto itr = std::lower_bound(keys.begin(), keys.end(), k);
        const int index = itr - keys.begin();

        keys.insert(itr, k);
        data.insert(data.begin() + index, d);
        //return index;
    }
};


// b+ tree
template <typename K, typename D>
class BPTree {
    std::unique_ptr< Node<K, D> *> root;

    const int b;

    // only called when splitting root
    void deorphan(std::unique_ptr<Node<K, D>> current) {
        // If no parent, then create a new parent
        if (current.parent == nullptr) {
            auto new_parent = std::make_unique<InternalNode<K, D> >();
            new_parent.is_leaf = false;
            new_parent.parent = nullptr;
            new_parent.children.push_back(current);

            current.parent = new_parent;
            root = new_parent;

        }
    }

    // splits internal node
    void split_internal(std::unique_ptr<InternalNode<K, D>> current) {
        deorphan(current);

        // Create sibling node
        auto new_node = std::make_unique<InternalNode<K, D> >();
        new_node.is_leaf = current.is_leaf;
        new_node.parent = current.parent;

        // Copy half of the keys (not middle) and half of the children (including middle)
        // middle key will MOVE up to the parent later
        const int mid = current.keys.size() / 2;
        new_node.children.emplace_back(std::move(current.children[mid]));
        for (int i = mid + 1; i < current.keys.size(); ++i) {
            new_node.keys.emplace_back(std::move(current.keys[i]));
            new_node.children.emplace_back(std::move(current.children[i]));
        }

        // Remove the keys we copied
        current.keys.erase(current.keys.end() - new_node.keys.size());
        current.children.erase(current.children.end() - new_node.children.size());

        // Handle splitting of ancestors
        if (current.parent.keys.size() == b) {
            split_internal(current.parent);
        }

        // Move middle key into parent
        current.parent.insert(current.keys[mid], new_node);
        current.keys.pop_back();
    }

    // splits leaf node
    void split_leaf(std::unique_ptr<LeafNode<K, D>> leaf) {
        deorphan(leaf);

        // Create sibling node
        auto new_node = std::make_unique<LeafNode<K, D> >();
        new_node.is_leaf = leaf.is_leaf;
        new_node.parent = leaf.parent;
        new_node.next = leaf.next;
        leaf.next = new_node;

        // Copy half of the keys and data
        const int mid = leaf.keys.size() / 2;
        for (int i = mid; i < leaf.keys.size(); ++i) {
            new_node.keys.emplace_back(std::move(leaf.keys[i]));
            new_node.data.emplace_back(std::move(leaf.data[i]));
        }

        // Remove the items we copied
        leaf.keys.erase(leaf.keys.end() - new_node.keys.size());
        leaf.data.erase(leaf.data.end() - new_node.data.size());

        // Handle splitting of internal nodes
        if (leaf.parent.keys.size() == b) {
            split_internal(leaf.parent);
        }

        // Add key and child for new_node to parent
        leaf.parent.insert(new_node.keys[0], leaf);
        // when splitting leaf, key is present in new_node AND parnet
    }

public:

    // insert single entry
    void insert(const K& k, std::unique_ptr<D> d) {

        // Create new node if needed
        if (root == nullptr) {
            root = std::make_unique<LeafNode<K, D>>();
            
            root.is_leaf = true;
            root.next = nullptr;
            root.parent = nullptr;
            root.keys.push_back(k);
            root.data.push_back(d);

            return;
        }

        // Otherwise search for the right node
        auto current = root;

        while (!current.is_leaf) {
            // Find the right child
            int i = 0;
            for (const auto key: current.keys) {
                if (k <= key) {
                    break;
                }
                ++i;
            }
            // Traverse onto the child node
            current = root.children[i];
        }

        // Perform splitting procedure if node is full
        if (current.keys.size() == b) {
            split_leaf(current);
        }
        
        current.insert(k, d); // valid as current should be leaf

        // TODO make sure "next" is updated properly
    }
};


int main() {
}
