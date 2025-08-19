#include <iostream>
#include <fstream>
#include <vector>
#include <fstream>
#include <algorithm>

const bool RED = true;
const bool BLACK = false;

using namespace std;

class TreeNode {
private:
    int childs, value, version, count;
    TreeNode *left, *right, *parent;
    bool color;
public:
    TreeNode(int number, int version, bool state) : value(number), left(nullptr), right(nullptr), parent(nullptr), color(state), version(version), count(1), childs(0) {}

    ~TreeNode() {}

    void PrintNode(int h) {
        char def[4] = "\t";
        if (right != nullptr) {
            right->PrintNode(h + 1);
        }
        else {
            for (int i = 0; i <= h; ++i) {
                cout << def;
            }
            cout << "{nullptr, b}\n";
        }

        for (int i = 0; i < h; ++i) {
            cout << def;
        }
        cout << "{ v:" << value << " c:" << childs << " c:" << count << " v:" << version;
        if (color == RED) {
            cout << ", r}\n";
        }
        else {
            cout << ", b}\n";
        }
        

        if (left != nullptr) {
            left->PrintNode(h + 1);
        }
        else {
            for (int i = 0; i <= h; ++i) {
                cout << def;
            }
            cout << "{nullptr, b}\n";
        }
    }

    friend class RedBlackTree;
};

class RedBlackTree {
private:
    int version; 
    vector<TreeNode*> root;

    inline bool Color(TreeNode* node) {
        if (node == nullptr) {
            return BLACK;
        } else {
            return node->color;
        }
    }

    inline int GetChilds(TreeNode* node) {
        if (node == nullptr) {
            return 0;
        }
        return node->childs + node->count;
    }

    inline void ChangeColor(TreeNode* grand_parent) {
        grand_parent->color = RED;
        grand_parent->right->color = BLACK;
        grand_parent->left->color = BLACK;
    }

    inline TreeNode* TreeFixRotateRight(TreeNode* grand_parent, TreeNode* child) {
        TreeNode* parent = grand_parent->left;
        if (parent->right == child) {
            parent = RotateLeft(parent);
            child = parent->left;
            grand_parent->left = parent;
        }
        RotateRight(grand_parent);
        parent->color = BLACK;
        grand_parent->color = RED;
        return parent;
    }

    inline TreeNode* TreeFixRotateLeft(TreeNode* &grand_parent, TreeNode* child) {
        TreeNode* parent = grand_parent->right;
        if (parent->left == child) {
            parent = RotateRight(parent);
            child = parent->right;
            grand_parent->right = parent;
        }
        RotateLeft(grand_parent);
        parent->color = BLACK;
        grand_parent->color = RED;
        return parent;
    }
   
    // Не перепревязывает себя у отца
    TreeNode* RotateLeft(TreeNode* parent) {
        TreeNode* child = parent->right;
        child->parent = parent->parent;
        parent->parent = child;
        parent->right = child->left;
        if (child->left != nullptr) {
            child->left->parent = parent;
        }
        child->left = parent;
        parent->childs = GetChilds(parent->left) + GetChilds(parent->right);
        child->childs = GetChilds(child->left) + GetChilds(child->right);
        return child;
    }

    // Не перепревязывает себя у отца
    TreeNode* RotateRight(TreeNode* parent) {
        TreeNode* child = parent->left;
        child->parent = parent->parent;
        parent->parent = child;
        parent->left = child->right;
        if (child->right != nullptr) {
            child->right->parent = parent;
        } 
        child->right = parent;
        parent->childs = GetChilds(parent->left) + GetChilds(parent->right);
        child->childs = GetChilds(child->left) + GetChilds(child->right);
        return child;
    }

    void CopyNodes(TreeNode* &parent, TreeNode* &uncle, TreeNode* &grand_parent) {
        TreeNode *new_parent, *new_grand_parent, *new_uncle;
        new_parent = CopyNode(parent);
        ReassignChilds(new_parent);

        if (uncle != nullptr) {
            new_uncle = CopyNode(uncle);
            ReassignChilds(new_uncle);
        } else {
            new_uncle = nullptr; 
        }

        new_grand_parent = CopyNode(grand_parent);
        if (grand_parent->left == parent) {
            new_grand_parent->left = new_parent;
            new_grand_parent->right = new_uncle;
        } else {
            new_grand_parent->right = new_parent;
            new_grand_parent->left = new_uncle;
        }
        ReassignChilds(new_grand_parent);

        grand_parent = new_grand_parent;
        parent = new_parent;
        if (uncle != nullptr) {
            uncle = new_uncle;
        }
    }

    TreeNode* CopyNode(TreeNode* current) {
        TreeNode *new_node = new TreeNode(current->value, version, current->color);
        new_node->left = current->left;
        new_node->right = current->right;
        new_node->parent = current->parent;
        new_node->count = current->count;
        new_node->childs = current->childs;
        return new_node;
    }

    void ReassignChilds(TreeNode* current) {
        if (current->left != nullptr) {
            current->left->parent = current;
        }
        if (current->right != nullptr) {
            current->right->parent = current;
        }
    }

    void FixAndCopyTree(TreeNode* current, TreeNode* parent, bool need_fixing) {
        while (parent != nullptr) {
            bool current_is_left = current->value < parent->value;

            if (need_fixing) {
                // Балансировка дерева
                TreeNode *grand_parent = parent->parent;
                if (grand_parent->left == parent) {
                    TreeNode *uncle = grand_parent->right;

                    // Копирование всех нод
                    CopyNodes(parent, uncle, grand_parent);
                    parent->childs = parent->childs + 1;
                    grand_parent->childs = grand_parent->childs + 1;
                    if (current_is_left) {
                        parent->left = current;
                    } else {
                        parent->right = current;
                    }
                    current->parent = parent;

                    if (Color(uncle) == RED) {
                        // Перекраска
                        ChangeColor(grand_parent);
                        current = grand_parent;
                        parent = grand_parent->parent;
                        if (Color(parent) == BLACK) {
                            need_fixing = false;
                        }
                    } else {
                        // Вращение
                        current = TreeFixRotateRight(grand_parent, current);
                        parent = current->parent;
                        need_fixing = false;
                    }
                } else {
                    TreeNode *uncle = grand_parent->left;

                    // Копирование всех нод
                    CopyNodes(parent, uncle, grand_parent);
                    parent->childs = parent->childs + 1;
                    grand_parent->childs = grand_parent->childs + 1;
                    if (current_is_left) {
                        parent->left = current;
                    } else {
                        parent->right = current;
                    }
                    current->parent = parent;

                    if (Color(uncle) == RED) {
                        // Перекраска
                        ChangeColor(grand_parent);
                        current = grand_parent;
                        parent = grand_parent->parent;
                        if (Color(parent) == BLACK) {
                            need_fixing = false;
                        }
                    } else {
                        // Вращение
                        current = TreeFixRotateLeft(grand_parent, current);
                        parent = current->parent;
                        need_fixing = false;
                    }
                }
                
            } else {
                // Простое копирование пути
                TreeNode *new_parent = new TreeNode(parent->value, version, parent->color);
                if (current_is_left) {
                    new_parent->right = parent->right;
                    new_parent->left = current;
                } else {
                    new_parent->right = current;
                    new_parent->left = parent->left;
                }
                current->parent = new_parent;
                new_parent->count = parent->count;
                new_parent->childs = parent->childs+1;
                ReassignChilds(new_parent);

                current = new_parent;
                parent = parent->parent;
            }
        }
        current->color = BLACK;
        current->parent = nullptr;
        root[version] = current;
    }

    inline TreeNode* GetRoot() {
        return root[version-1];
    }

public: 
    RedBlackTree(int capacity) : version(-1) {
        root = vector<TreeNode*>(capacity);
    }

    void Insert(int number) {
        ++version;
        if (version == 0) {
            auto node = new TreeNode(number, 0, BLACK);
            root[version] = node;
            return;
        }
        

        TreeNode *current = GetRoot();
        bool exist = false;
        while (true) {
            if (current->value == number) {
                exist = true;
                break;
            } 

            if (current->value < number) {
                if (current->right == nullptr) {
                    break;
                }
                current = current->right;
            } else {
                if (current->left == nullptr) {
                    break;
                }
                current = current->left;
            }
        }

        TreeNode *new_node = new TreeNode(number, version, RED);
        if (exist) {
            new_node->childs = current->childs;
            new_node->count = current->count+1;
            new_node->left = current->left;
            new_node->right = current->right;
            ReassignChilds(new_node);
            FixAndCopyTree(new_node, current->parent, false);
        } else {
            FixAndCopyTree(new_node, current, current->color == RED);
        }
    }

    int GetLENumbersCount(int version, int number) {
        TreeNode *current = root[version];
        int counter = 0;
        while (current != nullptr) {
            if (current->value == number) {
                return counter + GetChilds(current->left) + current->count;
            }
            if (current->value < number) {
                counter += GetChilds(current->left) + current->count;
                current = current->right;
            } else {
                current = current->left;
            }
        }
        return counter;
    }

    int GetLNumbersCount(int version, int number) {
        TreeNode *current = root[version];
        int counter = 0;
        while (current != nullptr) {
            if (current->value == number) {
                return counter + GetChilds(current->left);
            }
            if (current->value < number) {
                counter += GetChilds(current->left) + current->count;
                current = current->right;
            } else {
                current = current->left;
            }
        }
        return counter;
    }

    int GetVersionsCount() {
        return root.size();
    }

    void PrintTree(int version) {
        cout << "\n";
        root[version]->PrintNode(0);
        cout << "\n";
    }
};

int main() {
    ifstream in("input.txt");
    ofstream out("output.txt");
    int n, m; // Число отрезков и точек
    in >> n >> m;
    vector<pair<int, pair<int, int>>> otr(n);
    vector<int> heights(n);
    for (int i = 0; i < n; ++i) {
        int l, r, h;
        in >> l >> r >> h;
        otr[i] = {h, {l, r}}; 
        heights[i] = h;
    }
    RedBlackTree nach(n), kon(n);
    sort(otr.begin(), otr.end(), greater<>());
    sort(heights.begin(), heights.end());
    for (int i = 0; i < n; ++i) {
        nach.Insert(otr[i].second.first);
        kon.Insert(otr[i].second.second);
    }

    // Онлайн
    for (int i = 0; i < m; ++i) {
        int x, y;
        in >> x >> y;
        int first_index = upper_bound(heights.begin(), heights.end(), y) - heights.begin();
        if (first_index == heights.size()) {
            out << 0 << "\n";
            continue;
        } else {
            first_index = n - first_index - 1;
            out << nach.GetLENumbersCount(first_index, x) - kon.GetLNumbersCount(first_index, x) << "\n";
        }
    }
    return 0;
}