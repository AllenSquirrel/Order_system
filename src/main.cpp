#include"dp.hpp"
#include"httplib.h"

#define WWWROOT "./wwwroot/client_code"
using namespace httplib;

order_sys::TableDish *tb_dish=NULL;
order_sys::TableOrder *tb_order=NULL;

void DishInsert(const Request &req,Response &rsp)
{
  /*
   * 1,业务处理 解析正文（json反序列化string转对象）得到菜品信息  将菜品信息插入数据库
   *
   * 2，设置响应信息
   * */
  Json::Value dish;
  Json::Reader reader;
  bool ret=reader.parse(req.body,dish);//从body解析到dish对象存储
  if(ret==false)
  {
    //回复客户端错误
    Json::Value reason;
    Json::FastWriter writer;
    reason["result"]=false;
    reason["reason"]="dish info parse failed";
    rsp.status=400;
    rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
    return;
  }
  ret=tb_dish->Insert(dish);
  if(ret==false)
  {
    //回复客户端错误
    Json::Value reason;
    Json::FastWriter writer;
    reason["result"]=false;
    reason["reason"]="mysql insert failed";
    rsp.status=500;
    rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
    return;
  }
  rsp.status=200;
  return;
}

void DishDelete(const Request &req,Response &rsp)
{
  int dish_id=std::stoi(req.matches[1]);
  bool ret=tb_dish->Delete(dish_id);
  if(ret==false)
  {
    //回复客户端错误
    Json::Value reason;
    Json::FastWriter writer;
    reason["result"]=false;
    reason["reason"]="mysql delete failed";
    rsp.status=500;
    rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
    return;
  }
  rsp.status=200;
  return;
}

void DishUpdate(const Request &req,Response &rsp)
{
  int dish_id=std::stoi(req.matches[1]);
  Json::Value dish;
  Json::Reader reader;
  bool ret=reader.parse(req.body,dish);//从body解析到dish对象存储
  if(ret==false)
  {
    //回复客户端错误
    Json::Value reason;
    Json::FastWriter writer;
    reason["result"]=false;
    reason["reason"]="dish info parse failed";
    rsp.status=400;
    rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
    return;
  }
  dish["id"]=dish_id;//设置需要更改的对应dish对象的id
  ret=tb_dish->Update(dish);
  if(ret==false)
  {
    //回复客户端错误
    Json::Value reason;
    Json::FastWriter writer;
    reason["result"]=false;
    reason["reason"]="mysql Update failed";
    rsp.status=500;
    rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
    return;
  }
  return;
}
void DishGetAll(const Request &req,Response &rsp)
{
    Json::Value dishes;//创建新对象 接收获取菜品信息
    bool ret=tb_dish->SelectAll(&dishes);
    if(ret==false)
    {
      //回复客户端错误
      Json::Value reason;
      Json::FastWriter writer;
      reason["result"]=false;
      reason["reason"]="mysql Selectall failed";
      rsp.status=500;
      rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
      rsp.set_header("Content-Type","application/json");
      return;
    }
    Json::FastWriter writer;
    rsp.status=200;
    rsp.body=writer.write(dishes);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
    return;
}
void DishGetOne(const Request &req,Response &rsp)
{
    int dish_id=std::stoi(req.matches[1]);
    Json::Value dish;//创建新对象 接收获取菜品信息
    bool ret=tb_dish->SelectOne(dish_id,&dish);
    if(ret==false)
    {
      //回复客户端错误
      Json::Value reason;
      Json::FastWriter writer;
      reason["result"]=false;
      reason["reason"]="mysql Selectone failed";
      rsp.status=500;
      rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
      rsp.set_header("Content-Type","application/json");
      return;
    }
    Json::FastWriter writer;
    rsp.status=200;
    rsp.body=writer.write(dish);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
  return;
}
void OrderInsert(const Request &req,Response &rsp)
{
  Json::Value order;
  Json::Reader reader;
  bool ret=reader.parse(req.body,order);//从body解析到order对象存储
  if(ret==false)
  {
    //回复客户端错误
    Json::Value reason;
    Json::FastWriter writer;
    reason["result"]=false;
    reason["reason"]="order info parse failed";
    rsp.status=400;
    rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
    return;
  }
  ret=tb_order->Insert(order);
  if(ret==false)
  {
    //回复客户端错误
    Json::Value reason;
    Json::FastWriter writer;
    reason["result"]=false;
    reason["reason"]="mysql insert failed";
    rsp.status=500;
    rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
    return;
  }
  rsp.status=200;
  return;
}
void OrderDelete(const Request &req,Response &rsp)
{
  int order_id=std::stoi(req.matches[1]);
  bool ret=tb_order->Delete(order_id);
  if(ret==false)
  {
    //回复客户端错误
    Json::Value reason;
    Json::FastWriter writer;
    reason["result"]=false;
    reason["reason"]="mysql delete failed";
    rsp.status=500;
    rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
    return;
  }
  rsp.status=200;
  return;
}
void OrderUpdate(const Request &req,Response &rsp)
{
  int order_id=std::stoi(req.matches[1]);
  Json::Value order;
  Json::Reader reader;
  bool ret=reader.parse(req.body,order);//从body解析到order对象存储
  if(ret==false)
  {
    //回复客户端错误
    Json::Value reason;
    Json::FastWriter writer;
    reason["result"]=false;
    reason["reason"]="order info parse failed";
    rsp.status=400;
    rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
    return;
  }
  order["id"]=order_id;//设置需要更改的对应dish对象的id
  ret=tb_order->Update(order);
  if(ret==false)
  {
    //回复客户端错误
    Json::Value reason;
    Json::FastWriter writer;
    reason["result"]=false;
    reason["reason"]="mysql Update failed";
    rsp.status=500;
    rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
    return;
  }
  return;
}
void OrderGetAll(const Request &req,Response &rsp)
{
    Json::Value orderes;//创建新对象 接收获取订单信息
    bool ret=tb_order->SelectAll(&orderes);
    if(ret==false)
    {
      //回复客户端错误
      Json::Value reason;
      Json::FastWriter writer;
      reason["result"]=false;
      reason["reason"]="mysql Selectall failed";
      rsp.status=500;
      rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
      rsp.set_header("Content-Type","application/json");
      return;
    }
    Json::FastWriter writer;
    rsp.status=200;
    rsp.body=writer.write(orderes);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
  return;
}
void OrderGetOne(const Request &req,Response &rsp)
{
    int order_id=std::stoi(req.matches[1]);
    Json::Value order;//创建新对象 接收获取订单信息
    bool ret=tb_order->SelectOne(order_id,&order);
    if(ret==false)
    {
      //回复客户端错误
      Json::Value reason;
      Json::FastWriter writer;
      reason["result"]=false;
      reason["reason"]="mysql Selectone failed";
      rsp.status=500;
      rsp.body=writer.write(reason);//反序列化  转为json格式的string存储在body中
      rsp.set_header("Content-Type","application/json");
      return;
    }
    Json::FastWriter writer;
    rsp.status=200;
    rsp.body=writer.write(order);//反序列化  转为json格式的string存储在body中
    rsp.set_header("Content-Type","application/json");
  return;
}


int main()
{
  tb_dish=new order_sys::TableDish();
  tb_order=new order_sys::TableOrder();//定义全局变量指针，操作数据库，而每一个函数都可使用，更改同步
  Server server;
  server.set_base_dir(WWWROOT);
  server.Post("/dish",DishInsert);
  server.Delete(R"(/dish/(\d+))",DishDelete);
  server.Put(R"(/dish/(\d+))",DishUpdate);
  server.Get(R"(/dish)",DishGetAll);
  server.Get(R"(/dish/(\d+))",DishGetOne);
  server.Post("/order",OrderInsert);
  server.Delete(R"(/order/(\d+))",OrderDelete);
  server.Put(R"(/order/(\d+))",OrderUpdate);
  server.Get(R"(/order)",OrderGetAll);
  server.Get(R"(/order/(\d+))",OrderGetOne);
  
  server.listen("0.0.0.0",9021);
  return 0;
}
