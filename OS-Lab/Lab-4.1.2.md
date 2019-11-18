# malloc 的历史

碍于物理世界的限制，内存总是有限的。在这种情况下，动态内存分派就显得尤为重要。对于一段程序，在编译期间我们可能无从得知其实际上会使用多少内存，大部分的内存分派都需要在运行时进行。再加上 C 并没有 GC 等自动管理内存的基础设施。最终导致我们程序员必须自己管理全部的动态内存，对于 C 语言，我们需要使用标准规定的 `stdlib.h` 内部提供的几个内存管理函数来管理内存，这几个函数的信息如下：

```
NAME
 calloc, malloc, free, realloc - Allocate and free
 dynamic memory
SYNOPSIS
 #include <stdlib.h>
 void *calloc(size_t nmemb, size_t size);
 void *malloc(size_t size);
 void free(void *ptr);
 void *realloc(void *ptr, size_t size);
DESCRIPTION
 calloc() allocates memory for an array of nmemb elements
 of size bytes each and returns a pointer to the allo-
 cated memory. The memory is set to zero.
 malloc() allocates size bytes and returns a pointer to
 the allocated memory. The memory is not cleared.
 free() frees the memory space pointed to by ptr, which
 must have been returned by a previous call to malloc(),
 calloc() or realloc(). Otherwise, or if free(ptr) has
 already been called before, undefined behaviour occurs.
 If ptr is NULL, no operation is performed.
 realloc() changes the size of the memory block pointed
 to by ptr to size bytes. The contents will be unchanged
 to the minimum of the old and new sizes; newly allocated 
 memory will be uninitialized. If ptr is NULL, the call
 is equivalent to malloc(size); if size is equal to zero,
 the call is equivalent to free(ptr). Unless ptr is
 NULL, it must have been returned by an earlier call to
 malloc(), calloc() or realloc(). 
```

C 标准只是描述了函数的声明，其真正实现还得是由对应的标准库处理，本文将大致回顾下比较著名的 `malloc` 的实现。

### malloc 101

#### Linux 进程内存模型

![](https://www.programering.com/images/remote/ZnJvbT1jbmJsb2dzJnVybD1jbWJ3NXlNNGdqTTRNRE95TWpNekVUTTFFekwwQUROeEFqTXZNek54WUROMDhTYXYwMmJqNXladnhtWTBsbWJqNXljbGRXWXRsMkx2b0RjMFJIYQ.jpg)

在 Linux 下常见的进程的内存分布如上图。在 `x86` 机器上，默认最高 `1G` 空间是给内核保留的，最下面一部分是为了兼容做的保留区域以及进程的影响，在为一块巨大的地址空闲区。在空闲区的最上面是向下增长的栈区，栈顶通常保存在特定的寄存器上（x86 上是 rsp），在编译器进行栈的内存分配。空闲区的下面是向上增长的堆区，其内存的分配与回收在 C 语言内需要我们自己进行管理。

#### 进程内存管理 ： 操作系统视角

为了支持内存管理，操作系统必定需要提供对应的调用来为进程分配内存，实际上 `malloc` 也是使用操作系统提供的调用进行底层的内存分配的。在 Linux 下，内核给我们提供了 `sbrk` 以及 `mmap` 等系统调用来为进程分配虚拟内存。在操作系统的视角下，为进程分配内存的流程如下：

1. 使用 `sbrk` 等系统调用为进程分配一块**虚拟**地址空间
2. 在第一次访问被分配的地址时，触发缺页异常，操作系统开始分配物理内存，并为其建立虚拟内存和物理内存之间的映射关系

##### `sbrk` 与 `mmap` 之间的区别

- `sbrk` 的作用是将堆的上界（上图的 Program Break）向上推
- `mmap` 的作用是在堆与栈之间的空缺地带寻找一块空闲内存，并将这部分内存分配出来

事实上，我们可以直接使用系统调用来替代 `malloc` 的功能，例如这样：

```C
void* malloc(size_t size) {
    // Ask the system for more bytes by extending the heap space.
    void *p = sbrk(size);
    if(p == (void *)-1)
        return NULL; // No space left
    return p;
}
void free(void *ptr) {
      // Nothing
}
```

不过，这么实现有几个问题：

1. 众所周知进行系统调用的耗时相比直接的函数调用特别巨大，如果我们每分配一个字节的内存给进程时都要来一次系统调用的话，其开销显然是不可接受的。这一行为就像一个无能的领导遇到点鸡毛蒜皮的小事就要向上级汇报一样，尽管看起来没啥问题，但是长久以来肯定会被刷下去的。
2. 无法有效的重复利用内存，如此铺张浪费属实不应该

如果我们真的将这么一个垃圾分配器投入使用，我们的程序很快就会耗尽全部的内存然后被操作系统给干掉。为了解决这个问题，我们需要引入一些分配策略，使得内存分配器仅仅在需要的时候才向操作系统请求一整块内存，同时尽可能地重复使用已经分配了的内存。

#### 我们需要什么样的 malloc

在现代，我们常用的脚本语言/GUI 程序等东西似乎都很需要小规模但高频率的内存分配与回收，如何处理因为这样的使用方法造成的内存碎片是当今的内存分配器要处理的最重要的问题之一。

同时因为多处理器的流行，程序能否实现线程安全也是比较重要的一个部分，毕竟没有人想自己的高并发的程序卡在 `malloc` 上。

### Malloc : The K & R Approach

K & R 在他们著名的  《C 程序设计语言》 中描述了一份在现在看来仍然很简洁的 `malloc` 实现方式，其代码如下（[来源](https://stackoverflow.com/a/36512105)）：

```C
#include <unistd.h>  // sbrk

#define NALLOC 1024  // Number of block sizes to allocate on call to sbrk
#ifdef NULL
#undef NULL
#endif
#define NULL 0


// long is chosen as an instance of the most restrictive alignment type
typedef long Align;

/* Construct Header data structure.  To ensure that the storage returned by
 * kandr_malloc is aligned properly for the objects that are stored in it, all
 * blocks are multiples of the header size, and the header itself is aligned
 * properly.  This is achieved through the use of a union; this data type is big
 * enough to hold the "widest" member, and the alignment is appropriate for all
 * of the types in the union.  Thus by including a member of type Align, which
 * is an instance of the most restrictive type, we guarantee that the size of
 * Header is aligned to the worst-case boundary.  The Align field is never used;
 * it just forces each header to the desired alignment.
 */
union header {
  struct {
    union header *next;
    unsigned size;
  } s;

  Align x;
};
typedef union header Header;


static Header base;           // Used to get an initial member for free list
static Header *freep = NULL;  // Free list starting point


static Header *morecore(unsigned nblocks);
void kandr_free(void *ptr);


void *kandr_malloc(unsigned nbytes) {

  Header *currp;
  Header *prevp;
  unsigned nunits;

  /* Calculate the number of memory units needed to provide at least nbytes of
   * memory.
   *
   * Suppose that we need n >= 0 bytes and that the memory unit sizes are b > 0
   * bytes.  Then n / b (using integer division) yields one less than the number
   * of units needed to provide n bytes of memory, except in the case that n is
   * a multiple of b; then it provides exactly the number of units needed.  It
   * can be verified that (n - 1) / b provides one less than the number of units
   * needed to provide n bytes of memory for all values of n > 0.  Thus ((n - 1)
   * / b) + 1 provides exactly the number of units needed for n > 0.
   *
   * The extra sizeof(Header) in the numerator is to include the unit of memory
   * needed for the header itself.
   */
  nunits = ((nbytes + sizeof(Header) - 1) / sizeof(Header)) + 1;

  // case: no free list yet exists; we have to initialize.
  if (freep == NULL) {

    // Create degenerate free list; base points to itself and has size 0
    base.s.next = &base;
    base.s.size = 0;

    // Set free list starting point to base address
    freep = &base;
  }

  /* Initialize pointers to two consecutive blocks in the free list, which we
   * call prevp (the previous block) and currp (the current block)
   */
  prevp = freep;
  currp = prevp->s.next;

  /* Step through the free list looking for a block of memory large enough to
   * fit nunits units of memory into.  If the whole list is traversed without
   * finding such a block, then morecore is called to request more memory from
   * the OS.
   */
  for (; ; prevp = currp, currp = currp->s.next) {

    /* case: found a block of memory in free list large enough to fit nunits
     * units of memory into.  Partition block if necessary, remove it from the
     * free list, and return the address of the block (after moving past the
     * header).
     */
    if (currp->s.size >= nunits) {

      /* case: block is exactly the right size; remove the block from the free
       * list by pointing the previous block to the next block.
       */
      if (currp->s.size == nunits) {
    /* Note that this line wouldn't work as intended if we were down to only
     * 1 block.  However, we would never make it here in that scenario
     * because the block at &base has size 0 and thus the conditional will
     * fail (note that nunits is always >= 1).  It is true that if the block
     * at &base had combined with another block, then previous statement
     * wouldn't apply - but presumably since base is a global variable and
     * future blocks are allocated on the heap, we can be sure that they
     * won't border each other.
     */
    prevp->s.next = currp->s.next;
      }
      /* case: block is larger than the amount of memory asked for; allocate
       * tail end of the block to the user.
       */
      else {
    // Changes the memory stored at currp to reflect the reduced block size
    currp->s.size -= nunits;
    // Find location at which to create the block header for the new block
    currp += currp->s.size;
    // Store the block size in the new header
    currp->s.size = nunits;
      }

      /* Set global starting position to the previous pointer.  Next call to
       * malloc will start either at the remaining part of the partitioned block
       * if a partition occurred, or at the block after the selected block if
       * not.
       */
      freep = prevp;

      /* Return the location of the start of the memory, i.e. after adding one
       * so as to move past the header
       */
      return (void *) (currp + 1);

    } // end found a block of memory in free list case

    /* case: we've wrapped around the free list without finding a block large
     * enough to fit nunits units of memory into.  Call morecore to request that
     * at least nunits units of memory are allocated.
     */
    if (currp == freep) {
      /* morecore returns freep; the reason that we have to assign currp to it
       * again (since we just tested that they are equal), is that there is a
       * call to free inside of morecore that can potentially change the value
       * of freep.  Thus we reassign it so that we can be assured that the newly
       * added block is found before (currp == freep) again.
       */
      if ((currp = morecore(nunits)) == NULL) {
    return NULL;
      }
    } // end wrapped around free list case
  } // end step through free list looking for memory loop
}




static Header *morecore(unsigned nunits) {

  void *freemem;    // The address of the newly created memory
  Header *insertp;  // Header ptr for integer arithmatic and constructing header

  /* Obtaining memory from OS is a comparatively expensive operation, so obtain
   * at least NALLOC blocks of memory and partition as needed
   */
  if (nunits < NALLOC) {
    nunits = NALLOC;
  }

  /* Request that the OS increment the program's data space.  sbrk changes the
   * location of the program break, which defines the end of the process's data
   * segment (i.e., the program break is the first location after the end of the
   * uninitialized data segment).  Increasing the program break has the effect
   * of allocating memory to the process.  On success, brk returns the previous
   * break - so if the break was increased, then this value is a pointer to the
   * start of the newly allocated memory.
   */
  freemem = sbrk(nunits * sizeof(Header));
  // case: unable to allocate more memory; sbrk returns (void *) -1 on error
  if (freemem == (void *) -1) {
    return NULL;
  }

  // Construct new block
  insertp = (Header *) freemem;
  insertp->s.size = nunits;

  /* Insert block into the free list so that it is available for malloc.  Note
   * that we add 1 to the address, effectively moving to the first position
   * after the header data, since of course we want the block header to be
   * transparent for the user's interactions with malloc and free.
   */
  kandr_free((void *) (insertp + 1));

  /* Returns the start of the free list; recall that freep has been set to the
   * block immediately preceeding the newly allocated memory (by free).  Thus by
   * returning this value the calling function can immediately find the new
   * memory by following the pointer to the next block.
   */
  return freep;
}




void kandr_free(void *ptr) {

  Header *insertp, *currp;

  // Find address of block header for the data to be inserted
  insertp = ((Header *) ptr) - 1;

  /* Step through the free list looking for the position in the list to place
   * the insertion block.  In the typical circumstances this would be the block
   * immediately to the left of the insertion block; this is checked for by
   * finding a block that is to the left of the insertion block and such that
   * the following block in the list is to the right of the insertion block.
   * However this check doesn't check for one such case, and misses another.  We
   * still have to check for the cases where either the insertion block is
   * either to the left of every other block owned by malloc (the case that is
   * missed), or to the right of every block owned by malloc (the case not
   * checked for).  These last two cases are what is checked for by the
   * condition inside of the body of the loop.
   */
  for (currp = freep; !((currp < insertp) && (insertp < currp->s.next)); currp = currp->s.next) {

    /* currp >= currp->s.ptr implies that the current block is the rightmost
     * block in the free list.  Then if the insertion block is to the right of
     * that block, then it is the new rightmost block; conversely if it is to
     * the left of the block that currp points to (which is the current leftmost
     * block), then the insertion block is the new leftmost block.  Note that
     * this conditional handles the case where we only have 1 block in the free
     * list (this case is the reason that we need >= in the first test rather
     * than just >).
     */
    if ((currp >= currp->s.next) && ((currp < insertp) || (insertp < currp->s.next))) {
      break;
    }
  }

  /* Having found the correct location in the free list to place the insertion
   * block, now we have to (i) link it to the next block, and (ii) link the
   * previous block to it.  These are the tasks of the next two if/else pairs.
   */

  /* case: the end of the insertion block is adjacent to the beginning of
   * another block of data owned by malloc.  Absorb the block on the right into
   * the block on the left (i.e. the previously existing block is absorbed into
   * the insertion block).
   */
  if ((insertp + insertp->s.size) == currp->s.next) {
    insertp->s.size += currp->s.next->s.size;
    insertp->s.next = currp->s.next->s.next;
  }
  /* case: the insertion block is not left-adjacent to the beginning of another
   * block of data owned by malloc.  Set the insertion block member to point to
   * the next block in the list.
   */
  else {
    insertp->s.next = currp->s.next;
  }

  /* case: the end of another block of data owned by malloc is adjacent to the
   * beginning of the insertion block.  Absorb the block on the right into the
   * block on the left (i.e. the insertion block is absorbed into the preceeding
   * block).
   */
  if ((currp + currp->s.size) == insertp) {
    currp->s.size += insertp->s.size;
    currp->s.next = insertp->s.next;
  }
  /* case: the insertion block is not right-adjacent to the end of another block
   * of data owned by malloc.  Set the previous block in the list to point to
   * the insertion block.
   */
  else {
    currp->s.next = insertp;
  }

  /* Set the free pointer list to start the block previous to the insertion
   * block.  This makes sense because calls to malloc start their search for
   * memory at the next block after freep, and the insertion block has as good a
   * chance as any of containing a reasonable amount of memory since we've just
   * added some to it.  It also coincides with calls to morecore from
   * kandr_malloc because the next search in the iteration looks at exactly the
   * right memory block.
   */
  freep = currp;
}
```

在 K & R 的实现中，经由 `sbrk` 直接返回的内存被称为 `core` ，当你多次使用 `malloc` / `free` 进行内存的分配与回收时，部分 `core` 内的部分内存可能处于被使用的状态、也可能处于被释放的状态。在 K & R 的实现中，他们使用一个**环形单链表**来保存还没有被使用的部分的起始地址。因为单链表使用的这部分内存在进程的别的部分看来是没有被分配的，我们在创建新的节点时候并不需要为其搞出一块新的内存。在调用`malloc` 函数时，K & R 的实现首先会遍历链表来找到足够大的空闲空间以供分配，如果不够会调用`morecore` 函数向操作系统索取更多的内存。在找到足够大的空间后，`malloc` 根据情况修改链表：如果找到的空闲空间恰好和需要的空间大小一致，那么就把这个节点从链表中移除掉；如果空闲空间比较大，就从这段空间的最后面取出所需大小将其地址返回，同时修改节点的大小。`free` 操作基本上就是将空间返还到链表的过程，不过由于需要进行链表的合并操作，导致看起来比较复杂。

K & R 的 `malloc` 的优点就是简单好实现。因为其在处理很小的内存的分配和回收时效率很高（只有一个 `core` 时甚至可以达到 O(1)），且在全程内存分配/回收仅有几十次时可以完美工作，这一内存分配器仍在嵌入式领域发挥着余热。

其缺陷也很明显，如果我们持续地分配很小块的内存，很快就会出现内存碎片的问题。再就是因为 `free` 时候需要进行合并操作，其时间复杂度达到了 O(n)。这两条结合起来就导致其面对小额高频的内存分配/回收时候很无力。该分配器也无法实现线程安全。这些缺陷时期不再适合现代操作系统，其归宿也许只能是之前提到了嵌入式领域了。

