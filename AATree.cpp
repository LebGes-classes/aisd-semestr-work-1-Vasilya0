#include <iostream>
#include <vector>
#include <algorithm>

template <typename K, typename V>
class AANode {
public:
    int level; // уровень узла
    K key;
    V value;
    AANode<K, V>* left; // левый ребенок
    AANode<K, V>* right; // правый ребенок

    AANode(K key, V value) : level(1), key(key), value(value), left(nullptr), right(nullptr) {}

    AANode(K key, V value, AANode<K, V>* nullNode) : level(1), key(key), value(value), left(nullNode), right(nullNode) {}
};

template <typename K, typename V>
class AATree {
private:
    AANode<K, V>* root; // корень дерева
    AANode<K, V>* nullNode; // level = 0

public:
    AATree() {
        nullNode = new AANode<K, V>(K(), V());
        nullNode->level = 0;
        nullNode->left = nullNode;
        nullNode->right = nullNode;
        root = nullNode;
    }

    ~AATree() {
        // TODO: Реализовать деструктор для очистки памяти
    }

    AANode<K, V>* skew(AANode<K, V>* node) { // косое вращение, выравнивание "левый поворот"
        if (node->level == node->left->level) { // когда левый ребенок имеет тот же уровень, что и родитель
            AANode<K, V>* leftChild = node->left; // левый ребенок становится новым корнем поддерева
            node->left = leftChild->right;
            leftChild->right = node;
            return leftChild;
        }
        return node;
    }

    AANode<K, V>* split(AANode<K, V>* node) { // разделение, "правый поворот"
        if (node->level == node->right->right->level) { // две последовательные равные связи с одинаковым уровнем
            AANode<K, V>* rightChild = node->right; // правый ребенок становится новым корнем поддерева
            node->right = rightChild->left;
            rightChild->left = node;
            rightChild->level++;
            return rightChild;
        }
        return node;
    }

    void insert(K key, V value) {
        root = insert(root, key, value); // Обновляем root
    }

private:
    AANode<K, V>* insert(AANode<K, V>* node, K key, V value) { // вставка
        if (node == nullNode || node == nullptr) {
            return new AANode<K, V>(key, value, nullNode); // создаем новый узел, если дерево пустое
        }

        if (key < node->key) {
            node->left = insert(node->left, key, value);
        }
        else if (key > node->key) {
            node->right = insert(node->right, key, value);
        }
        else {
            node->value = value; // если ключи совпадают, обновляем значение
        }

        node = skew(node); // балансировка после вставки
        node = split(node);
        return node;
    }

public:
    void deleteKey(K key) {
        root = deleteNode(root, key);
    }

private:
    AANode<K, V>* deleteNode(AANode<K, V>* node, K key) { // удаление
        if (node == nullNode) { // если дерево пустое, делать нечего
            return nullNode;
        }

        if (key < node->key) { // если меньше, проходимся слева, если больше - справа
            node->left = deleteNode(node->left, key);
        }
        else if (key > node->key) {
            node->right = deleteNode(node->right, key);
        }
        else { // если ключи совпали, проверяем детей
            if (node->left == nullNode && node->right == nullNode) { // если нет детей, просто удаляем
                delete node;
                return nullNode;
            }
            else if (node->left == nullNode) { // если один ребенок, заменяем удаляемый узел на него
                AANode<K, V>* temp = node->right;
                delete node;
                return temp;
            }
            else if (node->right == nullNode) {
                AANode<K, V>* temp = node->left;
                delete node;
                return temp;
            }
            else { // если оба ребенка, находим мин узел в правом поддереве, копируем его данные в узел и удаляем
                AANode<K, V>* minNode = findMinNode(node->right);
                node->key = minNode->key;
                node->value = minNode->value;
                node->right = deleteNode(node->right, minNode->key);
            }
        }

        if (node->left->level < node->level - 1 || node->right->level < node->level - 1) { // если после удаления узел слишком большой по сравнению с детьми, уменьшаем его
            node->level--;
            if (node->right->level > node->level) {
                node->right->level = node->level;
            }
        }

        node = skew(node); // балансировка после удаления
        node = split(node);
        return node;
    }

public:
    AANode<K, V>* findMinNode(AANode<K, V>* node) { // поиск минимального узла (вспомогательный метод)
        if (node == nullNode) {
            return nullNode; // дерево пустое
        }

        // идём влево, пока не дойдем до листа
        while (node->left != nullNode) {
            node = node->left;
        }

        return node;
    }

    V get(K key) { // поиск
        AANode<K, V>* node = root;
        while (node != nullNode) {
            if (key < node->key) {
                node = node->left;
            }
            else if (key > node->key) {
                node = node->right;
            }
            else {
                return node->value;
            }
        }
        return V(); // возвращаем значение по умолчанию, если не найдено
    }

    int size() {
        return size(root); // вызывает приватный метод size(AANode<K, V>* node)
    }

private:
    int size(AANode<K, V>* node) { // кол-во узлов в дереве
        if (node == nullNode) { // пустое дерево
            return 0;
        }
        int leftSize = size(node->left); // левое поддерево
        int rightSize = size(node->right); // правое поддерево
        return 1 + leftSize + rightSize; // размер (текущий узел + потомки)
    }

public:
    int getHeight() {
        return getHeight(root); // вызывает приватный метод getHeight(AANode<K, V>* node)
    }

private:
    int getHeight(AANode<K, V>* node) { // высота дерева
        if (node == nullNode) { // пустое дерево
            return 0;
        }

        int leftHeight = getHeight(node->left);
        int rightHeight = getHeight(node->right);
        return 1 + std::max(leftHeight, rightHeight);
    }

    void inOrderTraversal(AANode<K, V>* node, std::vector<K>& result) { // симметричный обход: Левый поддерев → Корень → Правое поддерево
        if (node == nullNode) {
            return;
        }

        inOrderTraversal(node->left, result);
        result.push_back(node->key);
        inOrderTraversal(node->right, result);
    }

public:
    std::vector<K> inOrderTraversal() {
        std::vector<K> result;
        inOrderTraversal(root, result);
        return result;
    }

private:
    void preOrderTraversal(AANode<K, V>* node, std::vector<K>& result) { // Прямой обход: Корень → Левое поддерево → Правое поддерево
        if (node == nullNode) {
            return;
        }
        result.push_back(node->key);
        preOrderTraversal(node->left, result);
        preOrderTraversal(node->right, result);
    }

public:
    std::vector<K> preOrderTraversal() {
        std::vector<K> result;
        preOrderTraversal(root, result);
        return result;
    }

private:
    void postOrderTraversal(AANode<K, V>* node, std::vector<K>& result) { // Обратный обход: Левое поддерево → Правое поддерево → Корень
        if (node == nullNode) {
            return;
        }
        postOrderTraversal(node->left, result);
        postOrderTraversal(node->right, result);
        result.push_back(node->key);
    }

public:
    std::vector<K> postOrderTraversal() {
        std::vector<K> result;
        postOrderTraversal(root, result);
        return result;
    }
};

int main() {
    AATree<int, int> tree;

    // Вставка элементов
    tree.insert(1, 23);
    tree.insert(2, 42);
    tree.insert(3, 10);

    // Поиск элемента
    int value = tree.get(2);
    std::cout << "Значение для ключа 2: " << value << std::endl; //  42

    // Обход дерева
    std::vector<int> inOrder = tree.inOrderTraversal();
    std::cout << "Симметричный обход: ";
    for (int key : inOrder) {
        std::cout << key << " ";
    }
    std::cout << std::endl; // [1, 2, 3]

    std::vector<int> preOrder = tree.preOrderTraversal();
    std::cout << "Прямой обход: ";
    for (int key : preOrder) {
        std::cout << key << " ";
    }
    std::cout << std::endl; // [2, 1, 3]

    std::vector<int> postOrder = tree.postOrderTraversal();
    std::cout << "Обратный обход: ";
    for (int key : postOrder) {
        std::cout << key << " ";
    }
    std::cout << std::endl; // [1, 3, 2]

    // Размер и высота дерева
    std::cout << "Размер дерева: " << tree.size() << std::endl; // 3
    std::cout << "Высота дерева: " << tree.getHeight() << std::endl; // 2

    // Удаление элемента
    tree.deleteKey(2);
    inOrder = tree.inOrderTraversal();
    std::cout << "После удаления ключа 2: ";
    for (int key : inOrder) {
        std::cout << key << " ";
    }
    std::cout << std::endl; // [1, 3]

    return 0;
}