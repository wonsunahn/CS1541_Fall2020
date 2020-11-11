#ifndef MEMOBJ_H
#define MEMOBJ_H

#include <glib.h>
#include <stdint.h>
#include <assert.h>
#include <vector>
#include <map>
#include <string>

class MemRequest;

/** @brief A generic memory object.
 *
 * Provides only abstract interfaces for access, toString, getStatString,
 * getContentString methods so can't be instantiated.  Children classes
 * Cache and DRAM override these methods to implement them respectively.
 * Has a memObjs global registry of created objects so that a memory object
 * is not created twice when referred to twice as lower level memory.
 */
class MemObj {
  protected:
    /** The global memory object registry */
    static std::map<std::string, MemObj*> memObjs;

    /** The name of the cache object on the config file */
    std::string name;
    /** The name of the lower level MemObj on the config file */
    gchar *lowerLevel;
    /** The lower level MemObj on the config file */
    MemObj *lowerLevelMemObj;

  public:
    /** Returns the named memory object.  If it is already created and the
     * name exists in the memObjs hash registry, then just returns that
     * object.  Othewise, reads in various parameters from the config file and
     * uses them to create a memory object of the appropriate type.  The new
     * object is added to the memObjs hash registry and then returned.
     *
     * @param name - The name of the memory object in the config file.
     */
    static MemObj *create(const char *name);

    /** Frees all objects in the memObjs registry to prevent memory leaks. */
    static void freeAll();
    /** Prints the descriptions of all objects in the memObjs registry. */
    static void printAll();
    /** Prints the access statistics of all objects in the memObjs registry. */
    static void printAllStats();
    /** Prints the contents of all caches in the memObjs registry. */
    static void printAllContents();

    /** Constructor.  Parses the lower level name from the config file and if
     * it is not "null", then recursively invokes MemObj::create to create the
     * lower level object.  The created object is assigned to
     * lowerLevelMemObj.
     *
     * @param name - The name of the memory object on the config file.
     */
    MemObj(const char *name);

    /** Destructor. */
    virtual ~MemObj();

    /** Returns the name of the MemObj on the config file. */
    std::string getName() const { return name; }
    /** Returns the name of the lower level MemObj on the config file. */
    std::string getLowerLevel() const { return lowerLevel; }
    /** Returns the lower level MemObj on the config file */
    MemObj *getLowerLevelMemObj() const { return lowerLevelMemObj; }

    /** Accesses memory object with memory request.
     *
     * @param mreq - The memory request
     */
    virtual void access(MemRequest *mreq) = 0;
    /** Returns a string that describes the MemObj */
    virtual std::string toString() const = 0;
    /** Returns a string that summarizes access statistics */
    virtual std::string getStatString() const = 0;
    /** Returns a string that dumps all valid lines in cache */
    virtual std::string getContentString() const = 0;
};

#endif // MEMOBJ_H
