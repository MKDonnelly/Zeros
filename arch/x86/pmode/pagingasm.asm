;Assembly routines for dealing with paging

;Disables paging for the system
global arch_disable_paging
arch_disable_paging:
   mov edx, cr0
   and edx, 0x7fffffff
   mov cr0, edx
   ret

;Enables paging. Make sure CR3 has
;teh address for the page_directory_t*!
global arch_enable_paging
arch_enable_paging:
   mov edx, cr0
   or edx, 0x80000000
   mov cr0, edx
   ret

;Invalidate a page. Needed whenever we change
;the page mapping without switching out CR3
global inval_page
inval_page:
   invlpg [esp+0x4]
   ret

;Returns the address of the page fault
global get_pfault_addr
get_pfault_addr:
   mov eax, cr2
   ret   

; Used to load a page directory
; First argument is pointer to pd_t*
global load_pd
load_pd:
   mov eax, [esp+4]
   mov cr3, eax
   call arch_enable_paging

   ret
