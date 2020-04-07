#include <iostream>
#include <string>
#include <deque>
#include <algorithm>
#include <vector>
#include <fstream>
#include <cstring>
#include <iomanip>
using namespace std;
vector<pair<string, string>> code; //存放最终编码
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
void codeLeaf(Node *root, string str)
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
            codeLeaf(root->lchild, str);
            str.pop_back();
        }
        if (root->rchild != NULL)
        {
            str += "1";
            codeLeaf(root->rchild, str);
            str.pop_back();
        }
    }
}
//对一棵树的叶子节点进行排序
void Tree::sortLeafs()
{
    sort(this->leafs.begin(), this->leafs.end());
}
//判断pair<string,int> temp中是否存在某个字符,若存在,自加1，否则添加该字符
void addTemp(vector<pair<string, int>> &temp, const string &str)
{
    for (vector<pair<string, int>>::iterator it = temp.begin(); it != temp.end(); it++)
    {
        if (it->first == str)
        {
            (it->second)++;
            return;
        }
    }
    temp.push_back(pair<string, int>(str, 1));
}
//统计字符出现次数，并存入temp数组
void countChar(const char *filename, vector<pair<string, int>> &temp)
{
    fstream myfile; //用来读取文件
    int total = 0;
    myfile.open(filename);
    char buffer[1024] = {0}; //缓冲器，存放每一行的内容
    while (!(myfile.eof()))
    {
        myfile.getline(buffer, 1024);
        for (int i = 0; i < 1024; i++)
        {
            string str;
            if (isalpha(buffer[i]))
            {
                str.push_back(buffer[i]);
                addTemp(temp, str);
                total++;
            }
        }
    }
    myfile.close();
    sort(temp.begin(), temp.end());
    cout << "字符的总个数为： " << total << " 个\t"
         << "字符的种类一共有： " << temp.size() << " 种\n"
         << endl;
}
//根据vector<pair<string, int>>数据计算各个字符出现的概率并存入deque<pair<float, string>>中
void calcFreq(deque<pair<float, string>> &freq, vector<pair<string, int>> &temp)
{
    int total = 0;
    for (vector<pair<string, int>>::iterator it = temp.begin(); it != temp.end(); it++)
    {
        total += it->second;
    }
    for (vector<pair<string, int>>::iterator it = temp.begin(); it != temp.end(); it++)
    {
        float p = ((float)(it->second) / (float)total);
        freq.push_back(pair<float, string>(p, it->first));
    }
}
//输出所有字符的编码
void showCode()
{
    cout << "\n各个字符的Huffman编码如下：" << endl;
    for (vector<pair<string, string>>::iterator it = code.begin(); it != code.end(); it++)
    {
        cout << it->first << "\t" << it->second << endl;
    }
}
//根据输入字符返回其Huffman编码
string getCode(string str)
{
    for (vector<pair<string, string>>::iterator it = code.begin(); it != code.end(); it++)
    {
        if (str == it->first)
        {
            return it->second;
        }
    }
    return str;
}
//对读入文本编码并输出编码后的文本
void enCoding(const char *filename1, const char *filename2)
{
    fstream myfile;     //用来读取文件
    fstream codingFile; //用来存放编码文件
    myfile.open(filename1);
    codingFile.open(filename2, ios::out);
    char buffer[1024] = {0}; //缓冲器，存放每一行的内容
    while (!(myfile.eof()))
    {
        myfile.getline(buffer, 1024);
        for (int i = 0; i < 1024; i++)
        {
            string str;
            str.push_back(buffer[i]);
            cout << getCode(str);
            codingFile << getCode(str);
        }
    }
    myfile.close();
    codingFile.close();
}
int main()
{
    vector<pair<string, int>> temp;
    deque<pair<float, string>> freq;
    countChar("originFile.txt", temp); //统计各个字符出现次数
    calcFreq(freq, temp);              //计算各个字符出现的概率

    cout << "各个字符出现的次数统计如下：" << endl;
    for (vector<pair<string, int>>::iterator it = temp.begin(); it != temp.end(); it++)
    {
        cout << it->first << "\t" << it->second << "次" << endl;
    }

    cout << "各个字符出现的概率统计如下：" << endl;
    for (deque<pair<float, string>>::iterator it = freq.begin(); it != freq.end(); it++)
    {
        cout << it->second << "\t" << it->first << endl;
    }

    Tree tr = Tree(freq);           //将字符加入树的叶子中
    tr.buildHfTree();               //构造Huffman树
    codeLeaf(tr.root, "");          //编码
    sort(code.begin(), code.end()); //排序
    showCode();
    enCoding("originFile.txt", "codingFile.txt");
    system("pause");
    return 0;
}
