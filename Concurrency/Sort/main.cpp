#include <iostream>
#include <random>
#include <bits/algorithmfwd.h>
#include <bits/stl_algo.h>


using namespace std;

//用于两个数交换位置
void Swap(int& lhs,int& rhs)
{
    int tmp = rhs;
    rhs = lhs;
    lhs = tmp;
}

//插入排序
void InsertionSort(int arr[],int n)
{
    //分别用于外层循环和内层循环
    int i,j;
    //用于记录下一个待比较的数
    int next = 0;
    //从第二个数开始比较（认为第一个数本身已经有序）
    for(i = 1;i < n;i++)
    {
        //下一个待处理的数就是i指向的那个
        next = arr[i];
        //开始插入过程，找到合适的插入位置并移动元素
        //j用于记录比较的每一个元素
        for(j = i - 1;j >= 0 && arr[j] > next;j--)
            //将元素后移
            arr[j + 1] = arr[j];
        //找到合适的位置后插入
        arr[j + 1] = next;
    }
}

//简单选择排序
void SelectionSort(int arr[],int n)
{
    //分别用外层与内层循环
    int i,j;
    //开始循环选择最小的
    for(i = 0;i < n - 1;i++)
        for (j = i + 1; j < n; j++)
            //如果找到更小的就交换两者
            if (arr[j] < arr[i])
                Swap(arr[j], arr[i]);
}

//冒泡排序
void BubbleSort(int arr[],int n)
{
    int i,j;
    //外层循环负责从最大值处向低处走，索引大于i的已被排好序
    for(i = n - 1;i >= 0;i--)
        //将最大的冒到最上面
        for(j = 1;j <= i;j++)
            if(arr[j-1] > arr[j])
                Swap(arr[j],arr[j-1]);
}

//快速排序
void QuickSort(int arr[],int low,int high)
{
    if(low > high)
        return;
    //设置轴元素值
    int pivot = arr[low];
    //设置两方向计数
    int left = low;
    int right = high;
    //扫描一遍数组，将轴元素放至正确位置
    while(left < right)
    {
        //先将right向下移动，找到第一个比轴元素小的元素
        while(left < right && arr[right] >= pivot)
            --right;
        //如果找到则直接将该元素赋值给arr[low]因为arr[low]值被pivot存储
        if(left < right)
            arr[left] = arr[right];
        //再将low向上移动，找到第一个比轴元素大的元素
        while(left < right && arr[left] <= pivot)
            ++left;
        //赋值
        if(left < right)
            arr[right] = arr[left];
    }
    //给轴元素赋值
    arr[left] = pivot;
    //此时left和right已经相等
    //递归左右两部分
    QuickSort(arr,low,left - 1);
    QuickSort(arr,right + 1,high);
}

//堆排序部分

//调整为大顶堆，待调整的堆只有堆顶元素不满足条件
// 参数分别为待调整的数组、根元素索引(从零开始)以及数组长度
void HeapAdjust(int arr[],int root,int length)
{
    //堆顶节点值
    int rootVal = arr[root];
    //表示当前的左孩子节点
    //由于数组从0开始所以需要计算出的值加一
    int child = root*2 + 1;
    //开始比较过程
    while(child <= length)
    {
        //先找到更大的那个孩子节点
        if(child + 1 < length && arr[child] < arr[child+1])
            child++;
        //然后比较孩子节点与父节点值，如果父节点大则不用调整直接退出
        if(arr[child] < rootVal)
            break;
        //否则直接把孩子节点赋值给父节点
        else {
            arr[root] = arr[child];
            //修改父节点索引以便继续循环
            root = child;
            child = 2*root + 1;
        }
        //交换后的父节点值
        arr[root] = rootVal;
    }
}

//开始堆排序
void HeapSort(int arr[],int n)
{
    //首先进行堆的初始化，从最后一个叶节点的根节点开始
    //依次循环至根节点
    for(int i = n/2 - 1;i >= 0;--i)
        HeapAdjust(arr,i,n);
    //然后每次读出堆顶元素值即可
    for(int i = n - 1;i > 0;i--)
    {
        //删除堆顶元素
        cout << arr[0] << endl;
        //将最后一个元素放到堆顶
        arr[0] = arr[i];
        //调整堆
        HeapAdjust(arr,0,i);
    }
}

//


int main(int argc,char* argv[])
{
    int length = stoi(argv[1]);
    int* numList = new int[length];
    default_random_engine engine;
    uniform_int_distribution<int> u(20,100);
    //初始化该数组
    for(int c = 0;c < length;c++)
    {
        numList[c] = u(engine);
        cout << numList[c] << " ";
    }
    //排序部分
    cout << endl;
    HeapSort(numList,length);
    //输出结果
    //for(int c = 0;c < length;c++)
        //cout << numList[c] << " ";
    return 0;
}