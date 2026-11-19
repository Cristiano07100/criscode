#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>


//实现安全生产者消费者

class person
{
public:
	
	int age;
	

};


//共享区
std::queue<person> q;

const int queueMaxSize = 100;
std::condition_variable notfull; //队列不满
//次数
int num;
//c++不用初始化
//全局互斥量
std::mutex mu;
//全局条件变量
std::condition_variable notempty;//队列不空
//判断生产者是否结束
bool done = false;
//生产者
const int mnum = 1000;
void producer()
{
	for (int i = 0; i < mnum; i++)
	{
		//生产商品
		person p;
		p.age = i;
		//上锁
		std::unique_lock<std::mutex> lock(mu);
		//判断队列是否满
		while (q.size() >= queueMaxSize)
		{
			notfull.wait(lock);
		}
		//添加
		q.push(p);
		//解锁
		lock.unlock();
		notempty.notify_one();
	}
	std::lock_guard<std::mutex> lock(mu);
	done = true;
	notempty.notify_all();
}

//消费者
void consumer()
{
	while (true)
	{
		//c++配合条件变量的加锁
		std::unique_lock<std::mutex> lock(mu);
		while (q.empty()&&!done)
		{
			notempty.wait(lock);
		}
		if (q.empty() && done) break; //消费者不在循环
		//拿货
		num++;
		q.pop();
		notfull.notify_one();
		
		//解锁
		lock.unlock();
		
	}
}

int main()
{
	
	std::thread p1(producer);//生产者
	std::thread c1(consumer);//消费者1
	std::thread c2(consumer);//消费者2

	p1.join();
	c1.join();
	c2.join();
	std::cout << num;
	return 0;
}
