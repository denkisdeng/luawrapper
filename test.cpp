#include "luaWrapper.h"
#include <stdio.h>

typedef lWrapper::luatable luatable;
typedef lWrapper::luaObject luaObject;
#define any_cast lWrapper::any_cast

//测试直接调用C函数
static int showmsg(const char *msg)
{
	printf("c-showmsg:%s\n",msg);
	return 0;
}

void test_call_c_function(lua_State *L)
{
	printf("-------call c function---------\n");
	lWrapper::register_function(L,"cshow",showmsg);
	try{
		lWrapper::call<void>("test1",L);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
}

class test_class_A
{
	public:
		test_class_A():memb_a(0)
		{
			printf("test_class_A void\n");
		}
		test_class_A(const test_class_A &other):memb_a(other.memb_a)
		{
			printf("test_class_A copy constructor %d \n",memb_a);
		}		
		~test_class_A()
		{
			printf("~test_class_A\n");
		}
		int  memb_a;
		void show()
		{
			printf("test_class_A::show\n");
		}
};

void test_class1(lua_State *L)
{
	printf("\n-----pass a c++ pointer to lua-----\n");
	lWrapper::register_class<test_class_A>::_register(L,"test_class_A")
		.constructor<void>()//无参构造
		.constructor<const test_class_A&>()//一个参数构造
		.property("memb_a",&test_class_A::memb_a)
		.function("show",&test_class_A::show);
	
	test_class_A a;
	a.memb_a = 100;
	try{
		lWrapper::call<void>("test2",L,&a);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
	printf("after call test2,a.memb_a:%d\n",a.memb_a);
}

void test_pass_luatable(lua_State *L)
{
	printf("\n-----pass luatable to lua-----\n");
	try{
		luatable lt;
		for(int i = 0; i < 10; ++i)
			lt.push_back(i);
		lWrapper::call<void>("test3",L,lt);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
}

luatable c_return_luatable()
{
	luatable lt;
	for(int i = 0; i < 10; ++i)
		lt.push_back(i);
	return lt;
}

void test_c_return_luatable(lua_State *L)
{
	printf("\n-----C++ return a luatable to lua-----\n");
	lWrapper::register_function(L,"c_return_luatable",c_return_luatable);
	try{
		lWrapper::call<void>("test4",L);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
}

void test_lua_return_luatable(lua_State *L)
{
	printf("\n-----lua return a luatable to C++-----\n");
	try{
		luatable lt = lWrapper::call<luatable>("test5",L);
		for(int i = 0 ;i < (int)lt.size(); ++i)
			printf("%s\n",any_cast<std::string>(lt[i]).c_str());
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}	
}

void test_op_lua_obj(lua_State *L)
{
	printf("\n----- operate lua object -----\n");
	try{
		luaObject lo = lWrapper::call<luaObject>("test6",L);
		lo.call<void>("show");
		printf("balance:%d\n",lo.Get<int>("balance"));
		lo.Set("balance",1000);
		printf("balance:%d\n",lo.Get<int>("balance"));		
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
}

class base
{
	public:
		virtual void show()
		{
			printf("this base show\n");
		}
		
		void show2()
		{
			printf("this is not virtual function\n");
		}
};

class child:public base
{
public:
	void show()
	{
		printf("this child show\n");
	}
};

void test_call_virtual_function(lua_State *L)
{
	printf("\n----- lua call C++ virtual function -----\n");
	lWrapper::register_class<base>::_register(L,"base")
		.function("show",&base::show)
		.function("show2",&base::show2);
	lWrapper::register_class<child,base>::_register(L,"child");
	child c;
	base *b = &c;
	try{
		lWrapper::call<void>("test7",L,b,&c);	
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}	
}

void test_int64(lua_State *L)
{
	printf("\n-----use 64 integer in lua-----\n");
	try{
		int64_t a = 4294967295;
		int64_t b = 4294967296;
		lWrapper::call<void>("test8",L,a,b);	
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}	
}

void test_pass_c_object(lua_State *L)
{
	printf("\n-----pass a C++ Object to lua-----\n");
	test_class_A a;
	a.memb_a = 100;
	try{
		lWrapper::call<void>("test9",L,a);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
	printf("after test9,a.memb_a:%d\n",a.memb_a);
}

void arg_c_object(test_class_A a)
{
	printf("%d\n",a.memb_a);
	a.memb_a = 101;
}

void test_call_c_pass_obj(lua_State *L)
{
	printf("\n-----pass a C++ reference to lua-----\n");
	test_class_A a;
	a.memb_a = 100;
	lWrapper::register_function(L,"arg_c_object",arg_c_object);
	try{
		lWrapper::call<void>("test10",L,a);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
	printf("after test10,a.memb_a:%d\n",a.memb_a);
}

void arg_c_object_ref(test_class_A &a)
{
	printf("%d\n",a.memb_a);
	a.memb_a = 101;
}

void test_call_c_pass_obj_ref(lua_State *L)
{
	printf("\n-----pass a C++ reference to lua-----\n");
	test_class_A a;
	a.memb_a = 100;
	lWrapper::register_function(L,"arg_c_object_ref",arg_c_object_ref);
	try{
		lWrapper::call<void>("test11",L,a);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
	printf("after test11,a.memb_a:%d\n",a.memb_a);
}

void test_create_c_obj(lua_State *L)
{
	printf("\n-----construct C++ object in lua-----\n");
	try{
		lWrapper::call<void>("test12",L);
	}catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}	
}

void test_getglobal(lua_State *L)
{
	printf("\n-----lua global objcet-----\n");
	try{
		luatable lt = lWrapper::luaGetGlobal<luatable>(L,"t_table");
		for(int i = 0; i < (int)lt.size();++i)
		{
			printf("%d\n",any_cast<int>(lt[i]));
		}
	}
	catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
}

void test_setglobal(lua_State *L)
{
	printf("\n-----lua global object-----\n");
	try{
		
		lWrapper::luaSetGlobal(L,"TEST_GLOBAL","this is test global");
		lWrapper::call<void>("test13",L);
	}
	catch(std::string &err)
	{
		printf("%s\n",err.c_str());
	}
}



int main()
{
	lWrapper::luaWrapper lw;
	lw.dofile("start.lua");
	test_call_c_function(lw);
	test_class1(lw);
	test_pass_luatable(lw);
	test_c_return_luatable(lw);
	test_lua_return_luatable(lw);
	test_op_lua_obj(lw);
	test_call_virtual_function(lw);
	test_int64(lw);
	test_pass_c_object(lw);
	test_create_c_obj(lw);
	test_getglobal(lw);
	test_setglobal(lw);

	test_call_c_pass_obj(lw);
	test_call_c_pass_obj_ref(lw);
	getchar();
	return 0;
}
