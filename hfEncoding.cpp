#include <iostream>
#include <string>
#include <deque>
#include <algorithm>
#include <vector>
using namespace std;
vector<pair<string, string>> static code; //存放最终编码
struct Node
{
    Node *parent, *lchild, *rchild;
    pair<float, string> value;
    string ch;
};
class Tree
{
public:
    int max;
    deque<pair<float, string>> leafs; //存放所有叶子
    Node *root;
    void buildHfTree();                         //将所有叶子组合成哈夫曼树
    Tree(deque<pair<float, string>>);           //构造函数
    bool findLeaf(const pair<float, string> &); //查找叶子
    bool delLeaf(const pair<float, string> &);  //删除叶子
    void sortLeafs();
};
//重载pair的加法运算
pair<float, string> operator+(pair<float, string> pr1, pair<float, string> pr2)
{
    return pair<float, string>(pr1.first + pr2.first, pr1.second + pr2.second);
}
//Tree的构造函数
Tree::Tree(deque<pair<float, string>> lf)
{
    int count = 0;
    for (deque<pair<float, string>>::iterator it = lf.begin(); it != lf.end(); it++)
    {
        this->leafs.push_front(*it);
        count++;
    }
    this->max = count;
    this->root = NULL;
}
//根据键值对判断是否存在该叶子
bool Tree::findLeaf(const pair<float, string> &pr)
{
    for (deque<pair<float, string>>::iterator it = this->leafs.begin(); it != this->leafs.end(); it++)
    {
        if ((*it) == pr)
        {
            return true;
        }
    }
    return false;
}
//根据键值对删除一个叶子
bool Tree::delLeaf(const pair<float, string> &pr)
{
    for (deque<pair<float, string>>::iterator it = this->leafs.begin(); it != this->leafs.end(); it++)
    {
        if ((*it) == pr)
        {
            pair<float, string> temp = this->leafs.back();
            while (temp != (*it))
            {
                this->leafs.pop_back();
                this->leafs.push_front(temp);
                temp = this->leafs.back();
            }
            this->leafs.pop_back();
            return true;
        }
    }
    return false;
}
//根据键值对删除中间元素队列deque<Node*>中的一个元素
void delNodeInTemp(deque<Node *> &temp, const pair<float, string> &pr)
{
    for (deque<Node *>::iterator it = temp.begin(); it != temp.end(); it++)
    {
        if ((*it)->value == pr)
        {
            Node *nd = temp.back();
            while (nd->value != pr)
            {
                temp.pop_back();
                temp.push_front(nd);
                nd = temp.back();
            }
            temp.pop_back();
            return;
        }
    }
}
//根据键值对找到中间元素队列中的一个节点并返回其迭代器
Node *findNodeInTemp(deque<Node *> &temp, const pair<float, string> &pr)
{
    for (deque<Node *>::iterator it = temp.begin(); it != temp.end(); it++)
    {
        if ((*it)->value == pr)
        {
            return *it;
        }
    }
    return NULL;
}
//判断某个节点是否存在于中间元素队列中
bool isInTemp(deque<Node *> &temp, const pair<float, string> &pr)
{
    for (deque<Node *>::iterator it = temp.begin(); it != temp.end(); it++)
    {
        if ((*it)->value == pr)
        {
            return true;
        }
    }
    return false;
}
//将所有的叶子节点构造为一颗哈夫曼树
void Tree::buildHfTree()
{
    deque<Node *> temp;
    temp.push_front(NULL);
    while (this->leafs.begin() != this->leafs.end())
    {
        //对所有叶子排序并取出概率最小的两个叶子节点
        this->sortLeafs();
        pair<float, string> pr1;
        pair<float, string> pr2;
        if (this->leafs.back() == this->leafs.front()) //若原队列中只有一个节点，则作特殊处理
        {
            pr1 = pr2 = this->leafs.front();
            this->leafs.pop_front();
        }
        else
        {
            pr1 = this->leafs.front();
            this->leafs.pop_front();
            pr2 = this->leafs.front();
            this->leafs.pop_front();
        }
        //第一次合并，特殊处理
        if (temp.front() == NULL)
        {
            temp.pop_front();
            Node *node = new Node;
            if (pr1 == pr2)
            {
                node->lchild = node->parent = node->rchild = NULL;
                node->value = pr1, node->ch = pr1.second;
            }
            else
            {
                Node *node1 = new Node;
                Node *node2 = new Node;
                node1->value = pr1, node2->value = pr2;
                node->value = pr1 + pr2;
                node1->lchild = node1->rchild = node2->rchild = node2->lchild = node->parent = NULL;
                node1->parent = node2->parent = node;
                node->lchild = node1, node->rchild = node2;
                node1->ch = pr1.second, node2->ch = pr2.second;
            }
            //将处理后的节点放进两个队列中
            this->leafs.push_front(node->value);
            temp.push_front(node);
        }
        else
        { //不是第一次了
            Node *node = new Node;
            if (pr1 == pr2) //只剩一个节点了而且是被处理过的，表明所有节点处理完毕，直接退出
            {
                break;
            }
            else
            {
                //新选出的两个节点都是之前已经处理后得到的中间值节点
                if (isInTemp(temp, pr1) && isInTemp(temp, pr2))
                {
                    Node *node1 = findNodeInTemp(temp, pr1);
                    Node *node2 = findNodeInTemp(temp, pr2);
                    node->value = pr1 + pr2;
                    node->parent = NULL;
                    node1->parent = node2->parent = node;
                    node->lchild = node1, node->rchild = node2;
                    this->delLeaf(pr1), this->delLeaf(pr2);
                    delNodeInTemp(temp, pr1), delNodeInTemp(temp, pr2); //删除选出来的两个节点
                    this->leafs.push_front(node->value);
                }
                else if (isInTemp(temp, pr1))
                {
                    Node *tp = findNodeInTemp(temp, pr1);
                    Node *node2 = new Node;
                    node2->value = pr2, node->value = pr1 + pr2;
                    node2->ch = pr2.second;
                    node2->rchild = node2->lchild = node->parent = NULL;
                    node2->parent = tp->parent = node;
                    node->rchild = node2, node->lchild = tp;
                    this->delLeaf(pr1), this->delLeaf(pr2);                        //删除选出来的节点
                    this->leafs.push_front(node->value), delNodeInTemp(temp, pr1); //将合并的节点放到生成树和原始集合中
                }
                else if (isInTemp(temp, pr2))
                {
                    Node *tp = findNodeInTemp(temp, pr2);
                    Node *node1 = new Node;
                    node1->value = pr1, node->value = pr1 + pr2;
                    node1->ch = pr1.second;
                    node1->rchild = node1->lchild = node->parent = NULL;
                    node1->parent = tp->parent = node;
                    node->lchild = node1, node->rchild = tp;
                    this->delLeaf(pr1), this->delLeaf(pr2);                        //删除选出来的节点
                    this->leafs.push_front(node->value), delNodeInTemp(temp, pr2); //将合并的节点放到生成树和原始集合中
                }
                else
                {
                    Node *node1 = new Node;
                    Node *node2 = new Node;
                    node->value = pr1 + pr2;
                    node->parent = NULL;
                    node1->value = pr1, node2->value = pr2;
                    node1->parent = node2->parent = node;
                    node->lchild = node1, node->rchild = node2;
                    node1->lchild = node2->lchild = node1->rchild = node2->rchild = node->parent = NULL;
                    node1->ch = pr1.second, node2->ch = pr2.second;
                    this->delLeaf(pr1), this->delLeaf(pr2); //删除选出来的两个节点
                    this->leafs.push_front(node->value);
                }
            }
            temp.push_front(node);
        }
    }
    this->root = temp.front();
}

//前序遍历一棵树并对所有叶子节点编码
void getCode(Node *root, string str)
{
    if (root != NULL)
    {
        if (root->lchild == NULL && root->rchild == NULL)
        {
            code.push_back(pair<string, string>(root->value.second, str));
        }
        if (root->lchild != NULL)
        {
            str += "0";
            getCode(root->lchild, str);
            str.pop_back();
        }
        if (root->rchild != NULL)
        {
            str += "1";
            getCode(root->rchild, str);
            str.pop_back();
        }
    }
}
//对一棵树的叶子节点进行排序
void Tree::sortLeafs()
{
    sort(this->leafs.begin(), this->leafs.end());
}
//输入一个字符，输出其编码
string encoding(vector<pair<string, string>> code, string str)
{
    for (vector<pair<string, string>>::iterator it = code.begin(); it != code.end(); it++)
    {
        if ((*it).first == str)
        {
            return (*it).second;
        }
    }
    return string("absence");
}
//判断一个字符是不是字母
bool isalpha(char ch)
{
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
    {
        return true;
    }
    return false;
}
int main()
{
    deque<pair<float, string>> temp;
    temp.push_front(pair<float, string>(0.5, "a1"));
    temp.push_front(pair<float, string>(0.3, "a2"));
    temp.push_front(pair<float, string>(0.2, "a3"));
    Tree tr = Tree(temp);
    tr.buildHfTree();
    getCode(tr.root, "");
    sort(code.begin(),code.end());
    cout<<"you can select string from: "<<endl;
    for(vector<pair<string, string>>::iterator it = code.begin(); it != code.end(); it++){
        cout<<(*it).first<<endl;
    }
    cout << "\n================================" << endl;
    cout << "input your char:";
    string tp;
    cin >> tp;
    string rt = encoding(code, tp);
    cout << rt << endl;
    system("pause");
    return 0;
}
