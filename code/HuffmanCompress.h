#include <string>
#include <cstring>
#include <QString>
using namespace std;
struct head
{
    int b;                 //字符
    long count;            //文件中该字符出现的次数
    long parent, lch, rch; // make a tree
    char bits[256];        // the huffuman code
};
long lencom,outlencom;
long lende,outlende;
struct head header[512], tmp; //节点树

//读取文件内容并将压缩内容写入另一个文档
int compress(string &filename, string &outputfile)
{
    char buf[512];
    unsigned char ch;
    long i, j, m, n, f;
    long min1, pt1, flength;
    FILE *ifp, *ofp; // ifp：读指针    ofp：写指针
    ifp = fopen(filename.c_str(), "rb"); //以二进制方式打开原始文件，并只允许读操作
    if (ifp == NULL)
    {
        return 0; //如果打开失败，则输出错误信息
    }
    ofp = fopen(outputfile.c_str(), "wb"); //打开压缩后存储信息的文件，并只允许写操作
    if (ofp == NULL)
    {
        return 0;
    }
    flength = 0;
    while (!feof(ifp))
    {
        fread(&ch, 1, 1, ifp);
        header[ch].count++;
        flength++;
    }
    flength--;
    header[ch].count--; // feof的特性

    for (i = 0; i < 512; i++) // HUFFMAN算法中初始节点的设置
    {
        if (header[i].count != 0)           //如果该字符的数量不为0
            header[i].b = (unsigned char)i; //则将结构体中的字符b赋值为字母i
        else
            header[i].b = -1;               //没出现就赋值为-1
        header[i].parent = -1;              //父亲初值为-1
        header[i].lch = header[i].rch = -1; //左右子树初值也为-1
    }

    for (i = 0; i < 256; i++) //将节点按出现次数排序(冒泡排序)（降序）
    {
        for (j = i + 1; j < 256; j++)
        {
            if (header[i].count < header[j].count)
            {
                tmp = header[i];
                header[i] = header[j];
                header[j] = tmp;
            }
        }
    }

    for (i = 0; i < 256; i++)
        if (header[i].count == 0)
            break;

    n = i;                  // n为出现的字符数量
    m = 2 * n - 1;          // m为huffman树的节点数
    for (i = n; i < m; i++) //构造huffman树，总共n个节点，在数组中的位置为n到2n-1（正好是n个） 正好从i=n开始全是空节点（权值为0）
    {
        min1 = 999999999;
        for (j = 0; j < i; j++)
        {                               //找最小节点
            if (header[j].parent != -1) //找出还未加入huffman树（即没有父亲节点）的节点
                continue;

            if (min1 > header[j].count) //找出当前权值最小的节点，并保存其权值
            {
                pt1 = j; // pt1保存节点位置，min1保存权值
                min1 = header[j].count;
                continue;
            }
        }
        header[i].count = header[pt1].count; //当前空节点的权值改为最小节点的权值
        header[pt1].parent = i;              //最小节点的父亲节点为i，相当于tn
        header[i].lch = pt1;                 //左子树为最小节点
        min1 = 999999999;
        for (j = 0; j < i; j++)
        { //找第二小节点
            if (header[j].parent != -1)
                continue;

            if (min1 > header[j].count)
            {
                pt1 = j;
                min1 = header[j].count;
                continue;
            }
        }
        header[i].count += header[pt1].count; //当前节点权值改为最小节点和次小节点的权值和
        header[i].rch = pt1;                  //右子树为第二小节点
        header[pt1].parent = i;               //第二小节点的父亲节点也为i
    }

    for (i = 0; i < n; i++) //构造huffman树，设置字符的编码
    {
        f = i; // i只用于计数，用f来代替i的操作
        header[i].bits[0] = 0;
        while (header[f].parent != -1) //若当前节点无父亲节点
        {
            j = f; //同理，j来代替f
            f = header[f].parent;
            if (header[f].lch == j)
            {
                j = strlen(header[i].bits);
                memmove(header[i].bits + 1, header[i].bits, j + 1);
                header[i].bits[0] = '0'; //类似于递归
            }
            else
            {
                j = strlen(header[i].bits);
                memmove(header[i].bits + 1, header[i].bits, j + 1);
                header[i].bits[0] = '1';
            }
        }
    }

    //下面的就是读原文件的每一个字符，按照设置好的编码替换文件中的字符
    fseek(ifp, 0, SEEK_SET); //将指针定在文件起始位置
    fseek(ofp, 8, SEEK_SET); //以8位二进制数为单位进行读取
    buf[0] = 0;
    f = 0;
    pt1 = 8;
    lencom=flength;

    while (!feof(ifp))
    {
        ch = fgetc(ifp); // 每次读取一个字符
        f++;
        for (i = 0; i < n; i++) //开始进行匹配
            if (ch == header[i].b)
                break;

        strcat(buf, header[i].bits); // buf中存取当前读入字符的编码
        j = strlen(buf);             // j为编码长度
        ch = 0;
        while (j >= 8)
        {                           //当剩余字符数量不小于8个时
            for (i = 0; i < 8; i++) //按照八位二进制数转化成十进制ASCII码写入文件一次进行压缩
                ch = (ch << 1) | (buf[i] == '1');

            fwrite(&ch, 1, 1, ofp); // size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
            pt1++;
            strcpy(buf, buf + 8); //相当于buf指针右移8位，便于存储下一个读入字符的8位编码
            j = strlen(buf);
        }
        if (f == flength) //文件中所有字符读完后跳出
            break;
    }
    if (j > 0)
    {
        strcat(buf, "00000000"); //对不足的位数进行补零
        for (i = 0; i < 8; i++)
            ch = (ch << 1) | (buf[i] == '1');

        fwrite(&ch, 1, 1, ofp);
        pt1++;
    }
    fseek(ofp, 0, SEEK_SET);                   //将编码信息写入存储文件
    fwrite(&flength, 1, sizeof(flength), ofp); //写入信息总长度（所有信息的编码总长度）
    fwrite(&pt1, sizeof(long), 1, ofp);        // 写入文件末尾，即文件终止位置的指针
    fseek(ofp, pt1, SEEK_SET);
    fwrite(&n, sizeof(long), 1, ofp); //字符个数
    for (i = 0; i < n; i++)
    {
        tmp = header[i];

        fwrite(&(header[i].b), 1, 1, ofp); //写入字符
        pt1++;
        ch = strlen(header[i].bits);
        fwrite(&ch, 1, 1, ofp); //写入编码长度
        pt1++;
        j = strlen(header[i].bits);

        if (j % 8 != 0) //当位数不满8时，对该数进行补零操作
            for (f = j % 8; f < 8; f++)
                strcat(header[i].bits, "0");

        while (header[i].bits[0] != 0)
        {
            ch = 0;
            for (j = 0; j < 8; j++)
                ch = (ch << 1) | (header[i].bits[j] == '1');

            strcpy(header[i].bits, header[i].bits + 8);
            fwrite(&ch, 1, 1, ofp); //将所得的编码信息写入文件
            pt1++;
        }
        header[i] = tmp;
    }
    fclose(ifp);
    fclose(ofp); //关闭文件
    outlencom=pt1+4;
    return 1; //返回压缩成功信息
}

//解压缩文件，并将解压后的内容写入新文件
int uncompress(string &filename, string &outputfile)
{
    char buf[255], bx[255];
    unsigned char ch;
    long i, j, m, n, f, p, l;
    long flength;
    int per = 10;
    int len = 0;
    FILE *ifp, *ofp;
    ifp = fopen(filename.c_str(), "rb"); //打开文件
    if (ifp == NULL)
        return 0; //若打开失败，则输出错误信息

    //读取原文件长
    if (outputfile.size() == 0)
        outputfile += "0";

    ofp = fopen(outputfile.c_str(), "wb"); //打开文件
    if (ofp == NULL)
        return 0;

    fseek(ifp, 0, SEEK_END);
    len = ftell(ifp);
    fseek(ifp, 0, SEEK_SET);
    lende=len;
    fread(&flength, sizeof(long), 1, ifp); //读取原文件长
    fread(&f, sizeof(long), 1, ifp);
    fseek(ifp, f, SEEK_SET);
    fread(&n, sizeof(long), 1, ifp); //读取原文件各参数
    for (i = 0; i < n; i++)
    {                                   //读取压缩文件内容并转换成二进制码
        fread(&header[i].b, 1, 1, ifp); //读取字符
        fread(&ch, 1, 1, ifp);          //读取字符对应编码长度
        p = (long)ch;
        header[i].count = p; //将编码长度存储到header[i].count
        header[i].bits[0] = 0;
        m = p / 8 + (p % 8 > 0); // m为编码所占字节数

        for (j = 0; j < m; j++)
        {                          //按字节读取内容
            fread(&ch, 1, 1, ifp); //读取编码对应的ASCLL码
            f = ch;
            _itoa(f, buf, 2); // ASCLL码转化为二进制字符串，存储到buf中
            f = strlen(buf);  // f为字符串长度
            for (l = 8; l > f; l--)
                strcat(header[i].bits, "0"); //位数不足，执行补零操作，在每个字节最前面补0

            strcat(header[i].bits, buf); //将该位添加到header[i].bits末尾
        }
        header[i].bits[p] = 0;
    }

    for (i = 0; i < n; i++)
    { //对header[].bits的位数进行排序，位数更小的出现频率更高，下标更小利于遍历
        for (j = i + 1; j < n; j++)
        {
            if (strlen(header[i].bits) > strlen(header[j].bits))
            {
                tmp = header[i];
                header[i] = header[j];
                header[j] = tmp;
            }
        }
    }

    p = strlen(header[n - 1].bits); // p为bits编码的最大位数
    fseek(ifp, 8, SEEK_SET);
    m = 0; //已解压的字符个数
    bx[0] = 0;

    while (1)
    {
        while (strlen(bx) < (unsigned int)p) //读取至少p位
        {
            fread(&ch, 1, 1, ifp);
            f = ch;
            _itoa(f, buf, 2); //整数转换为字符串
            f = strlen(buf);
            for (l = 8; l > f; l--)
                strcat(bx, "0"); //位数不足，执行补零操作，在每个字节最前面补0

            strcat(bx, buf); //将该位添加到bx末尾
        }
        for (i = 0; i < n; i++)
            if (memcmp(header[i].bits, bx, header[i].count) == 0) //将bx的前header[i].count位与header[i].bits比较，如果相同就break
                break;

        strcpy(bx, bx + header[i].count); // bx后移header[i].count位
        ch = header[i].b;                 //找到编码对应的字符
        fwrite(&ch, 1, 1, ofp);           //写入ch
        m++;                              //已解压的字符个数++
        if (m == flength)
            break;
    }
    fclose(ifp);
    fclose(ofp);
    outlende=flength;
    return 1; //输出成功信息
}
