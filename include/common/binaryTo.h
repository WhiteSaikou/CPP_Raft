
#include <type_traits>
#include <string>
#include <vector>
#include <list>
#include<map>
#include<set>
#include<fstream>
#include<iostream>
#include <string.h>


#include "log.h"
namespace binary
{

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value,const int>::type get_serialize_size(const T& var)
{
    return sizeof(T);
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value>::type serialize_to_buf(const T& var,char*& buf)
{
    if(buf == nullptr)
        return;
    using ValueType = T;
    int size = sizeof(ValueType);
    memcpy(buf,&var,size);
    buf = buf + sizeof(ValueType);
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value>::type deserialize_from_buf(T& var, char*& buf)
{
    if(buf == nullptr)
        return;
    using ValueType = T;
    int size = sizeof(ValueType);
    memcpy(&var,buf,size);
    buf = buf + sizeof(ValueType);
}

void serialize_to_buf(const std::string& var,char*& buf)
{
    if(buf == nullptr)
        return;
    int length(var.length());
    serialize_to_buf(length,buf);
    memcpy(buf,var.c_str(),length);
    buf = buf + length;
}

void deserialize_from_buf(std::string& var,char*& buf)
{
    if(buf == nullptr)
        return;
    int length = 0;
    deserialize_from_buf(length,buf);
    //std::cout << "    length = " << length << std::endl;
    char* c_ctr = new char[length + 1];
    memcpy(c_ctr,buf,length);
    c_ctr[length] = '\0';
    var = c_ctr;
    buf = buf + length;
    delete []c_ctr;
}

const int get_serialize_size(const std::string& var)
{
    return sizeof(int) + var.length();
}

/* ------------------------ STL PAIR BEGIN ------------------------ */
template<typename T1,typename T2>
void serialize_to_buf(const std::pair<T1,T2>& var,char*& buf)
{
    if(buf == nullptr)
        return;
    serialize_to_buf(var.first,buf);
    serialize_to_buf(var.second,buf);
}

template<typename T1,typename T2>
void deserialize_from_buf(std::pair<T1,T2>& var,char*& buf)
{
    if(buf == nullptr)
        return;
    deserialize_from_buf(var.first,buf);
    deserialize_from_buf(var.second,buf);
}

template<typename T1,typename T2>
const int get_serialize_size(const std::pair<T1,T2>& var)
{
    return get_serialize_size(var.first) + get_serialize_size(var.second);
}
/* ------------------------ STL PAIR END ------------------------ */


/* ------------------------ STL VECTOR / LIST BEGIN ------------------------ */
template<typename T>
void serialize_to_buf(const std::vector<T>& var,char*& buf)
{
    if(buf == nullptr)
        return;
    int size = var.size();
    serialize_to_buf(size,buf);
    for(auto iter : var)
        serialize_to_buf(iter,buf);
}

template<typename T>
void deserialize_from_buf(std::vector<T>& var,char*& buf)
{
    if(buf == nullptr)
        return;
    int size = 0;
    deserialize_from_buf(size,buf);
    var.clear();
    for(int i =0; i<size; i++)
    {
        T new_Element;
        deserialize_from_buf(new_Element,buf);
        var.push_back(new_Element);
    }
}

template<typename T>
const int get_serialize_size(const std::vector<T>& var)
{
    int Element_size = 0;
    for(auto iter: var)
    {
        Element_size += get_serialize_size(iter);
    }
    return sizeof(int) + Element_size;
}


template<typename T>
void serialize_to_buf(const std::list<T>& var,char*& buf)
{
    if(buf == nullptr)
        return;
    int size = var.size();
    serialize_to_buf(size,buf);
    for(auto iter : var)
        serialize_to_buf(iter,buf);
}

template<typename T>
void deserialize_from_buf(std::list<T>& var,char*& buf)
{
    if(buf == nullptr)
        return;
    int size = 0;
    deserialize_from_buf(size,buf);
    var.clear();
    for(int i =0; i<size; i++)
    {
        T new_Element;
        deserialize_from_buf(new_Element,buf);
        var.push_back(new_Element);
    }
}

template<typename T>
const int get_serialize_size(const std::list<T>& var)
{
    int Element_size = 0;
    for(auto iter: var)
    {
        Element_size += get_serialize_size(iter);
    }
    return sizeof(int) + Element_size;
}

/* ------------------------ STL VECTOR / LIST END ------------------------ */

/* ------------------------ STL MAP BEGIN ------------------------ */


template<typename kType, typename VType>
void serialize_to_buf(const std::map<kType,VType>& var,char*& buf)
{
    if(buf == nullptr)
        return;
    int size = var.size();
    serialize_to_buf(size,buf);
    for(auto iter : var)
        serialize_to_buf(iter,buf);
}

template<typename kType, typename VType>
void deserialize_from_buf(std::map<kType,VType>& var,char*& buf)
{
    if(buf == nullptr)
        return;
    int size = 0;
    deserialize_from_buf(size,buf);
    var.clear();
    for(int i =0; i<size; i++)
    {
        std::pair<kType,VType> new_Element;
        deserialize_from_buf(new_Element,buf);
        var.insert(new_Element);
    }
}

template<typename kType, typename VType>
const int get_serialize_size(const std::map<kType,VType>& var)
{
    int Element_size = 0;
    for(auto iter: var)
    {
        Element_size += get_serialize_size(iter);
    }
    return sizeof(int) + Element_size;
}

/* ------------------------ STL MAP END ------------------------ */

/* ------------------------ STL SET BEGIN ------------------------ */

template<typename T>
void serialize_to_buf(const std::set<T>& var,char*& buf)
{
    if(buf == nullptr)
        return;
    int size = var.size();
    serialize_to_buf(size,buf);
    for(auto iter : var)
        serialize_to_buf(iter,buf);
}

template<typename T>
void deserialize_from_buf(std::set<T>& var,char*& buf)
{
    if(buf == nullptr)
        return;
    int size = 0;
    deserialize_from_buf(size,buf);
    var.clear();
    for(int i =0; i<size; i++)
    {
        T new_Element;
        deserialize_from_buf(new_Element,buf);
        var.insert(new_Element);
    }
}

template<typename T>
const int get_serialize_size(const std::set<T>& var)
{
    int Element_size = 0;
    for(auto iter: var)
    {
        Element_size += get_serialize_size(iter);
    }
    return sizeof(int) + Element_size;
}


/* ------------------------ STL SET END ------------------------ */

template<typename T>
bool serialize(const T& var,std::string file_name)
{
    std::ofstream file(file_name, std::ios::binary);
    if (!file.is_open())
        return false;
    
    int size = get_serialize_size(var);
    int total_size = size + sizeof(int);
    char* buf = new char[total_size];
    char* buf_begin = buf;
    serialize_to_buf(size,buf);
    serialize_to_buf(var,buf);
    file.write(buf_begin, total_size);
    file.close();
    delete[] buf_begin;
    return true;
}

template<typename T>
bool deserialize(T& var,std::string file_name)
{
    std::ifstream file(file_name, std::ios::binary);
    if (!file.is_open())
        return false;

    char *head= new char[sizeof(int)];
    char *head_begin = head;
    int size = 0;
    file.read(head,sizeof(int));
    deserialize_from_buf(size,head);
    delete head_begin;

    char *buf = new char[size];
    char *buf_begin = buf;
    file.read(buf,size);
    deserialize_from_buf(var,buf);
    file.close();
    delete[] buf_begin;
    return true;
}

void serialize_to_buf(const raft::log::log_entry_func& var, char* &buf) {
    serialize_to_buf((int)var.op, buf);
    serialize_to_buf(var.key, buf);
    serialize_to_buf((int)var.value, buf);
}

void deserialize_from_buf(raft::log::log_entry_func& var, char* &buf) {
    if(buf == nullptr)
        return;
    int op;
    deserialize_from_buf(op, buf);
    var.op = (raft::log::OPTION) op;
    deserialize_from_buf(var.key, buf);
    deserialize_from_buf(var.value, buf);
}



}
