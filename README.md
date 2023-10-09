File-as-heap
============

File backed Heap object store, primarily targetted for Caching applications. 

Issues in traditional memory approaches
----------------------------------------
* Fragmentation occurs due to un-even memory requirements.
* Caches are always symmetric in nature and every cache object are of same shape and size. Hence memory fragmentation can be minimized in this.
* Famous memory allocators like libumem, hoard etc do not provide, file backing.
* Caches are meant to restore the state, hence maintaining off-line history is an important requirement.
* Even in case of file backing, deleted / timed-out sessions are not really deleted from the file.
* Hence over the perid of time, file size grow to enormous extent and we are forced to go for new file.

Features of cacheHeapMap
------------------------
* File backed heap. Restoration from the heap binary file is possible during init stage.
* Every object will be of same size. The total size of the file will be automatically word aligned.
* MMAP policy is used. Hence the File I/O bottleneck does not happen. This improves performance.
* The custom heap is aligned in buckets. Hence every bucket can be re-used, re-cycled. The file size of the heap file never increases. Still the recycle is achieved.
* Easy integration possible. The allocate / deallocate / walkHeap provides shares only the pointers. Hence the meta-data involving in this product is merely pointers. nothing else.


Features planned for future versions
------------------------------------
* HTTP based statistics reporting.
* Shared library based packaging.
* COM object. Reusable.

'''
======================================
