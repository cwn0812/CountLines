// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <mutex>
typedef std::mutex						QMutex;
typedef std::lock_guard< std::mutex >	QAutoLock;


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
