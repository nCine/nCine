#ifndef CLASS_NCINE_LUAUTILS
#define CLASS_NCINE_LUAUTILS

#include <stdint.h>
#include "common_macros.h"

struct lua_State;

namespace ncine {

namespace LuaUtils {
	DLL_PUBLIC void addFunction(lua_State *L, const char *name, int (*func)(lua_State *L));
	DLL_PUBLIC void createTable(lua_State *L, int narr, int nrec);
	DLL_PUBLIC void newTable(lua_State *L);
	DLL_PUBLIC void call(lua_State *L, int nargs, int nresults);
	DLL_PUBLIC void pop(lua_State *L, int n);
	DLL_PUBLIC void pop(lua_State *L);
	DLL_PUBLIC int getField(lua_State *L, int index, const char *name);
	DLL_PUBLIC void setField(lua_State *L, int index, const char *name);

	DLL_PUBLIC bool isNil(lua_State *L, int index);
	DLL_PUBLIC bool isTable(int type);
	DLL_PUBLIC bool isTable(lua_State *L, int index);

	DLL_PUBLIC size_t rawLen(lua_State *L, int index);
	DLL_PUBLIC int rawGeti(lua_State *L, int index, int64_t n);
	DLL_PUBLIC void rawSeti(lua_State *L, int index, int64_t i);

	DLL_PUBLIC void getGlobal(lua_State *L, const char *name);
	DLL_PUBLIC void setGlobal(lua_State *L, const char *name);

	template <class T> T retrieve(lua_State *L, int index) {}
	template <> DLL_PUBLIC double retrieve<double>(lua_State *L, int index);
	template <> DLL_PUBLIC float retrieve<float>(lua_State *L, int index);
	template <> DLL_PUBLIC int64_t retrieve<int64_t>(lua_State *L, int index);
	template <> DLL_PUBLIC uint64_t retrieve<uint64_t>(lua_State *L, int index);
	template <> DLL_PUBLIC int32_t retrieve<int32_t>(lua_State *L, int index);
	template <> DLL_PUBLIC uint32_t retrieve<uint32_t>(lua_State *L, int index);
	template <> DLL_PUBLIC const char *retrieve<const char *>(lua_State *L, int index);
	template <> DLL_PUBLIC bool retrieve<bool>(lua_State *L, int index);
	void *retrieveUserData(lua_State *L, int index); // not DLL_PUBLIC
	template <class T> T *retrieveUserData(lua_State *L, int index) { return static_cast<T *>(retrieveUserData(L, index)); }
	void assertArrayLength(lua_State *L, int index, int length); // utility function, not DLL_PUBLIC

	template <class T>
	void retrieveArray(lua_State *L, int index, int arrayIndex, int length, T *array)
	{
		assertArrayLength(L, index, arrayIndex + length);
		for (int i = 0; i < length; i++)
		{
			rawGeti(L, index, arrayIndex + i);
			array[i] = retrieve<T>(L, -1);
		}
	}
	template <class T>
	void retrieveArray(lua_State *L, int index, int length, T *array)
	{
		retrieveArray(L, index, 1, length, array);
	}

	template <class T> T retrieveField(lua_State *L, int index, const char *name) {}
	template <> DLL_PUBLIC double retrieveField<double>(lua_State *L, int index, const char *name);
	template <> DLL_PUBLIC float retrieveField<float>(lua_State *L, int index, const char *name);
	template <> DLL_PUBLIC int64_t retrieveField<int64_t>(lua_State *L, int index, const char *name);
	template <> DLL_PUBLIC uint64_t retrieveField<uint64_t>(lua_State *L, int index, const char *name);
	template <> DLL_PUBLIC int32_t retrieveField<int32_t>(lua_State *L, int index, const char *name);
	template <> DLL_PUBLIC uint32_t retrieveField<uint32_t>(lua_State *L, int index, const char *name);
	template <> DLL_PUBLIC const char *retrieveField<const char *>(lua_State *L, int index, const char *name);
	DLL_PUBLIC void retrieveField(lua_State *L, int index, const char *name, char *dest, size_t *length);
	template <> DLL_PUBLIC bool retrieveField<bool>(lua_State *L, int index, const char *name);
	DLL_PUBLIC void retrieveFieldTable(lua_State *L, int index, const char *name);
	DLL_PUBLIC void retrieveFieldFunction(lua_State *L, int index, const char *name);
	DLL_PUBLIC void retrieveFieldLightUserData(lua_State *L, int index, const char *name);

	template <class T>
	void retrieveFieldArray(lua_State *L, int index, const char *name, int arrayIndex, int length, T *array)
	{
		getField(L, index, name);
		assertArrayLength(L, index, arrayIndex + length);
		for (int i = 0; i < length; i++)
		{
			rawGeti(L, index, arrayIndex + i);
			array[i] = retrieve<T>(L, -1);
		}
	}

	template <class T>
	void retrieveFieldArray(lua_State *L, int index, const char *name, int length, T *array)
	{
		retrieveFieldArray(L, index, name, 1, length, array);
	}

	template <class T> bool tryRetrieveField(lua_State *L, int index, const char *name, T &value) { return false; }
	template <> DLL_PUBLIC bool tryRetrieveField<double>(lua_State *L, int index, const char *name, double &value);
	template <> DLL_PUBLIC bool tryRetrieveField<float>(lua_State *L, int index, const char *name, float &value);
	template <> DLL_PUBLIC bool tryRetrieveField<int64_t>(lua_State *L, int index, const char *name, int64_t &value);
	template <> DLL_PUBLIC bool tryRetrieveField<uint64_t>(lua_State *L, int index, const char *name, uint64_t &value);
	template <> DLL_PUBLIC bool tryRetrieveField<int32_t>(lua_State *L, int index, const char *name, int32_t &value);
	template <> DLL_PUBLIC bool tryRetrieveField<uint32_t>(lua_State *L, int index, const char *name, uint32_t &value);
	DLL_PUBLIC bool tryRetrieveField(lua_State *L, int index, const char *name, const char *value);
	template <> DLL_PUBLIC bool tryRetrieveField<bool>(lua_State *L, int index, const char *name, bool &value);
	DLL_PUBLIC bool tryRetrieveFieldTable(lua_State *L, int index, const char *name);
	DLL_PUBLIC bool tryRetrieveFieldFunction(lua_State *L, int index, const char *name);
	DLL_PUBLIC bool tryRetrieveFieldLightUserData(lua_State *L, int index, const char *name);

	template <class T>
	bool tryRetrieveFieldArray(lua_State *L, int index, const char *name, int arrayIndex, int length, T *array)
	{
		getField(L, index, name);
		if (isTable(L, -1))
		{
			retrieveArray<T>(L, -1, arrayIndex, length, array);
			return true;
		}
		return false;
	}

	template <class T>
	bool tryRetrieveFieldArray(lua_State *L, int index, const char *name, int length, T *array)
	{
		return tryRetrieveFieldArray(L, index, name, 1, length, array);
	}

	template <class T> T retrieveGlobal(lua_State *L, const char *name) {}
	template <> DLL_PUBLIC double retrieveGlobal<double>(lua_State *L, const char *name);
	template <> DLL_PUBLIC float retrieveGlobal<float>(lua_State *L, const char *name);
	template <> DLL_PUBLIC int64_t retrieveGlobal<int64_t>(lua_State *L, const char *name);
	template <> DLL_PUBLIC uint64_t retrieveGlobal<uint64_t>(lua_State *L, const char *name);
	template <> DLL_PUBLIC int32_t retrieveGlobal<int32_t>(lua_State *L, const char *name);
	template <> DLL_PUBLIC uint32_t retrieveGlobal<uint32_t>(lua_State *L, const char *name);
	template <> DLL_PUBLIC const char *retrieveGlobal<const char *>(lua_State *L, const char *name);
	DLL_PUBLIC void retrieveGlobal(lua_State *L, const char *name, char *dest, size_t *length);
	template <> DLL_PUBLIC bool retrieveGlobal<bool>(lua_State *L, const char *name);
	DLL_PUBLIC void retrieveGlobalTable(lua_State *L, const char *name);
	DLL_PUBLIC void retrieveGlobalFunction(lua_State *L, const char *name);
	DLL_PUBLIC void retrieveGlobalLightUserData(lua_State *L, const char *name);
	void assertIsTable(lua_State *L, const char *name); // utility function, not DLL_PUBLIC

	template <class T>
	void retrieveGlobalArray(lua_State *L, const char *name, int arrayIndex, int length, T *array)
	{
		getGlobal(L, name);
		assertIsTable(L, name);
		retrieveArray<T>(L, -1, arrayIndex, length, array);
	}

	template <class T>
	void retrieveGlobalArray(lua_State *L, const char *name, int length, T *array)
	{
		retrieveGlobalArray(L, name, 1, length, array);
	}

	template <class T> bool tryRetrieveGlobal(lua_State *L, const char *name, T &value) { return false; }
	template <> DLL_PUBLIC bool tryRetrieveGlobal<double>(lua_State *L, const char *name, double &value);
	template <> DLL_PUBLIC bool tryRetrieveGlobal<float>(lua_State *L, const char *name, float &value);
	template <> DLL_PUBLIC bool tryRetrieveGlobal<int64_t>(lua_State *L, const char *name, int64_t &value);
	template <> DLL_PUBLIC bool tryRetrieveGlobal<uint64_t>(lua_State *L, const char *name, uint64_t &value);
	template <> DLL_PUBLIC bool tryRetrieveGlobal<int32_t>(lua_State *L, const char *name, int32_t &value);
	template <> DLL_PUBLIC bool tryRetrieveGlobal<uint32_t>(lua_State *L, const char *name, uint32_t &value);
	DLL_PUBLIC bool tryRetrieveGlobal(lua_State *L, const char *name, const char *value);
	template <> DLL_PUBLIC bool tryRetrieveGlobal<bool>(lua_State *L, const char *name, bool &value);
	DLL_PUBLIC bool tryRetrieveGlobalTable(lua_State *L, const char *name);
	DLL_PUBLIC bool tryRetrieveGlobalFunction(lua_State *L, const char *name);
	DLL_PUBLIC bool tryRetrieveGlobalLightUserData(lua_State *L, const char *name);

	template <class T>
	bool tryRetrieveGlobalArray(lua_State *L, const char *name, int arrayIndex, int length, T *array)
	{
		getGlobal(L, name);
		if (isTable(L, -1))
		{
			retrieveArray<T>(L, -1, arrayIndex, length, array);
			return true;
		}
		return false;
	}

	template <class T>
	bool tryRetrieveGlobalArray(lua_State *L, const char *name, int length, T *array)
	{
		return tryRetrieveGlobalArray(L, name, 1, length, array);
	}

	DLL_PUBLIC void push(lua_State *L);
	DLL_PUBLIC void push(lua_State *L, double number);
	DLL_PUBLIC void push(lua_State *L, float number);
	DLL_PUBLIC void push(lua_State *L, int64_t integer);
	DLL_PUBLIC void push(lua_State *L, uint64_t integer);
	DLL_PUBLIC void push(lua_State *L, int32_t integer);
	DLL_PUBLIC void push(lua_State *L, uint32_t integer);
	DLL_PUBLIC void push(lua_State *L, const char *string);
	DLL_PUBLIC void push(lua_State *L, int (*func)(lua_State *L));
	DLL_PUBLIC void push(lua_State *L, bool boolean);
	DLL_PUBLIC void push(lua_State *L, void *lightuserdata);

	template <class T>
	void pushArray(lua_State *L, int index, int arrayIndex, int length, T *array)
	{
		for (int i = 0; i < length; i++)
		{
			push(L, array[i]);
			rawSeti(L, index, arrayIndex + i);
		}
	}

	template <class T>
	void pushArray(lua_State *L, int index, int length, T *array)
	{
		pushArray(L, index, 1, length, array);
	}

	DLL_PUBLIC void pushField(lua_State *L, const char *name);
	DLL_PUBLIC void pushField(lua_State *L, const char *name, double number);
	DLL_PUBLIC void pushField(lua_State *L, const char *name, float number);
	DLL_PUBLIC void pushField(lua_State *L, const char *name, int64_t integer);
	DLL_PUBLIC void pushField(lua_State *L, const char *name, uint64_t integer);
	DLL_PUBLIC void pushField(lua_State *L, const char *name, int32_t integer);
	DLL_PUBLIC void pushField(lua_State *L, const char *name, uint32_t integer);
	DLL_PUBLIC void pushField(lua_State *L, const char *name, const char *string);
	DLL_PUBLIC void pushField(lua_State *L, const char *name, int (*func)(lua_State *L));
	DLL_PUBLIC void pushField(lua_State *L, const char *name, bool boolean);
	DLL_PUBLIC void pushField(lua_State *L, const char *name, void *lightuserdata);

	template <class T>
	void pushArrayField(lua_State *L, const char *name, int arrayIndex, int length, T *array)
	{
		const bool found = tryRetrieveFieldTable(L, -1, name);
		if (found == false)
			createTable(L, length, 0);
		pushArray<T>(L, -2, arrayIndex, length, array);
	}

	template <class T>
	void pushArrayField(lua_State *L, const char *name, int length, T *array)
	{
		pushArrayField(L, name, 1, length, array);
	}

	DLL_PUBLIC void setGlobal(lua_State *L, const char *name, double number);
	DLL_PUBLIC void setGlobal(lua_State *L, const char *name, float number);
	DLL_PUBLIC void setGlobal(lua_State *L, const char *name, int64_t integer);
	DLL_PUBLIC void setGlobal(lua_State *L, const char *name, uint64_t integer);
	DLL_PUBLIC void setGlobal(lua_State *L, const char *name, int32_t integer);
	DLL_PUBLIC void setGlobal(lua_State *L, const char *name, uint32_t integer);
	DLL_PUBLIC void setGlobal(lua_State *L, const char *name, const char *string);
	DLL_PUBLIC void setGlobal(lua_State *L, const char *name, int (*func)(lua_State *L));
	DLL_PUBLIC void setGlobal(lua_State *L, const char *name, bool boolean);
	DLL_PUBLIC void setGlobal(lua_State *L, const char *name, void *lightuserdata);

	template <class T>
	void setGlobalArray(lua_State *L, const char *name, int arrayIndex, int length, T *array)
	{
		const bool found = tryRetrieveGlobalTable(L, name);
		if (found == false)
			createTable(L, length, 0);
		pushArray<T>(L, -2, arrayIndex, length, array);
		if (found == false)
			setGlobal(L, name);
	}

	template <class T>
	void setGlobalArray(lua_State *L, const char *name, int length, T *array)
	{
		setGlobalArray(L, name, 1, length, array);
	}
}

}

#endif
