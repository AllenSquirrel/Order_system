#include<iostream>
#include<string>
#include<mutex>
#include<mysql/mysql.h>
#include<jsoncpp/json/json.h>

namespace order_sys
{
#define mysql_server "127.0.0.1"
#define mysql_user "root"  
#define mysql_name "order_sys"

  static MYSQL* MysqlInit()
  {
    //mysql初始化
    //1初始化句柄
    MYSQL* mysql=NULL;
    mysql=mysql_init(NULL);
    if(mysql==NULL)
    {
      std::cout<<"mysql init failed\n";
    }
    //2连接服务器 
    if(mysql_real_connect(mysql,mysql_server,mysql_user,NULL,mysql_name,0,NULL,0)==NULL)
    {
      std::cout<<mysql_error(mysql)<<std::endl;
      return NULL;
    }
    //3设置字符集
    if(mysql_set_character_set(mysql,"utf8")!=0)
    {
      std::cout<<mysql_error(mysql)<<std::endl;
      return NULL;
    }
    //4选择数据库
    if(mysql_select_db(mysql, mysql_name)!=0)
    {
      std::cout<<mysql_error(mysql)<<std::endl;
      return NULL;
    }
    return mysql;
  }
  static void MysqlRelease(MYSQL * mysql)
  {
    //mysql销毁
    if(mysql!=NULL)
    {
      mysql_close(mysql);
    }
    return;
  }
  static bool MysqlQuery(MYSQL *mysql,const std::string &sql)
  {
    if(mysql_query(mysql,sql.c_str())!=0)
    {
      std::cout<<sql<<std::endl;
      std::cout<<mysql_error(mysql)<<std::endl;
      return false;
    }
    return true;
  }

  class TableDish{
    private:
      MYSQL* _mysql;
      std::mutex _mutex;
    public:
      TableDish()
      {
        _mysql=MysqlInit();
        if(_mysql==NULL)
        {
          exit(-1);
        }
      }
      ~TableDish()
      {
        MysqlRelease(_mysql);
        _mysql=NULL;
      }
      bool Insert(const Json::Value &dish)
      {
        //组织sql语句
#define DISH_INSERT "insert tb_dish values(null,'%s',%d,now());"
        char str_sql[4096]={0};//初始化一块内存存放按照一定形式组织的字符串
        sprintf(str_sql,DISH_INSERT,dish["name"].asCString(),dish["price"].asInt());
        //执行sql语句
        return MysqlQuery(_mysql,str_sql);
      }
      bool Delete(int dish_id)
      {
#define DISH_DELETE "delete from tb_dish where id=%d;"
        char str_sql[4096]={0};
        sprintf(str_sql,DISH_DELETE,dish_id);
        return MysqlQuery(_mysql,str_sql);
      }
      bool Update(const Json::Value &dish)
      {
#define DISH_UPDATE "update tb_dish set name='%s',price=%d where id=%d;"
        char str_sql[4096]={0};
        sprintf(str_sql,DISH_UPDATE,dish["name"].asCString(),dish["price"].asInt(),dish["id"].asInt());
        return MysqlQuery(_mysql,str_sql);
      }
      bool SelectAll(Json::Value *dishes)
      {
#define DISH_SELECT_ALL "select* from tb_dish;"
        _mutex.lock();
        bool ret=MysqlQuery(_mysql,DISH_SELECT_ALL);
        if(ret==false)
        {
          _mutex.unlock();
          return false;
        }
        MYSQL_RES *res=mysql_store_result(_mysql);
        _mutex.unlock();
        if(res==NULL)
        {
          std::cout<<"store result failed\n";
          return false;
        }
        int num=mysql_num_rows(res);
        for(int i=0;i<num;i++)
        {
          MYSQL_ROW row=mysql_fetch_row(res);
          Json::Value dish;
          dish["id"]=std::stoi(row[0]);
          dish["name"]=row[1];
          dish["price"]=std::stoi(row[2]);
          dish["ctime"]=row[3];
          dishes->append(dish);
        }
        mysql_free_result(res);
        return true;
      }
      bool SelectOne(int dish_id,Json::Value *dish)
      {
#define DISH_SELECT_ONE "select* from tb_dish where id=%d;"
        char str_sql[4096]={0};
        sprintf(str_sql,DISH_SELECT_ONE,dish_id);
        _mutex.lock();
        bool ret=MysqlQuery(_mysql,str_sql);
        if(ret==false)
        {
          _mutex.unlock();
          return false;
        }
        MYSQL_RES *res=mysql_store_result(_mysql);
        _mutex.unlock();
        if(res==NULL)
        {
          std::cout<<"store result failed\n";
          return false;
        }
        int num=mysql_num_rows(res);
        if(num!=1)
        {
          std::cout<<"select error\n";
          mysql_free_result(res);
          return false;
        }
          MYSQL_ROW row=mysql_fetch_row(res);
          (*dish)["id"]=std::stoi(row[0]);
          (*dish)["name"]=row[1];
          (*dish)["price"]=std::stoi(row[2]);
          (*dish)["ctime"]=row[3];
      mysql_free_result(res);
      return true;
      }
  };

  class TableOrder{
    private:
      MYSQL* _mysql;
      std::mutex _mutex;
    public:
      TableOrder()
      {
        _mysql=MysqlInit();
        if(_mysql==NULL)
        {
          exit(-1);
        }
      }
      ~TableOrder()
      {
        MysqlRelease(_mysql);
        _mysql=NULL;
      }

      bool Insert(const Json::Value &order)
      {
        //组织sql语句
#define ORDER_INSERT "insert tb_order values(null,'%s',0,now());"
        char str_sql[4096]={0};//初始化一块内存存放按照一定形式组织的字符串
        Json::FastWriter writer;
        std::string dishes=writer.write(order["dishes"]);//序列化 order对象转换为json格式string
        sprintf(str_sql,ORDER_INSERT,dishes.c_str());
        //执行sql语句
        return MysqlQuery(_mysql,str_sql);
       }
      bool Delete(int order_id)
      {
#define DISH_ORDER "delete from tb_order where id=%d;"
        char str_sql[4096]={0};
        sprintf(str_sql,DISH_ORDER,order_id);
        return MysqlQuery(_mysql,str_sql);
      }
      bool Update(const Json::Value &order)
      {
#define ORDER_UPDATE "update tb_order set dishes='%s',status=%d where id=%d;"
        Json::FastWriter writer;
        std::string dishes=writer.write(order["dishes"]);//序列化  json转string
        char str_sql[4096]={0};
        sprintf(str_sql,ORDER_UPDATE,dishes.c_str(),order["status"].asInt(),order["id"].asInt());
        return MysqlQuery(_mysql,str_sql);
      }
      bool SelectAll(Json::Value *orderes)
      {
#define ORDER_SELECT_ALL "select* from tb_order;"
        _mutex.lock();
        bool ret=MysqlQuery(_mysql,ORDER_SELECT_ALL);
        if(ret==false)
        {
          _mutex.unlock();
          return false;
        }
        MYSQL_RES *res=mysql_store_result(_mysql);
        _mutex.unlock();
        if(res==NULL)
        {
          std::cout<<"store result failed\n";
          return false;
        }
        int num=mysql_num_rows(res);
        for(int i=0;i<num;i++)
        {
          MYSQL_ROW row=mysql_fetch_row(res);
          Json::Value order;
          Json::Reader reader;
          Json::Value tmp;
          order["id"]=std::stoi(row[0]);
          //order["dishes"]=row[1];
          reader.parse(row[1],tmp);//反序列化 解析json格式字符串到order对象
          order["dishes"]=tmp;
          order["status"]=std::stoi(row[2]);
          order["ctime"]=row[3];
          orderes->append(order);
        }
        mysql_free_result(res);
        return true;
      }
      bool SelectOne(int order_id,Json::Value *order)
      {
#define ORDER_SELECT_ONE "select* from tb_order where id=%d;"
        char str_sql[4096]={0};
        sprintf(str_sql,ORDER_SELECT_ONE,order_id);
        _mutex.lock();
        bool ret=MysqlQuery(_mysql,str_sql);
        if(ret==false)
        {
          _mutex.unlock();
          return false;
        }
        MYSQL_RES *res=mysql_store_result(_mysql);
        _mutex.unlock();
        if(res==NULL)
        {
          std::cout<<"store result failed\n";
          return false;
        }
        int num=mysql_num_rows(res);
        if(num!=1)
        {
          std::cout<<"select error\n";
          mysql_free_result(res);
          return false;
        }
          MYSQL_ROW row=mysql_fetch_row(res);
          (*order)["id"]=std::stoi(row[0]);
         // (*order)["dishes"]=row[1];
          Json::Reader reader;
          Json::Value tmp;
          reader.parse(row[1],tmp);
          (*order)["dishes"]=tmp;
          (*order)["status"]=std::stoi(row[2]);
          (*order)["mtime"]=row[3];
      mysql_free_result(res);
      return true;
      }
  };
}
