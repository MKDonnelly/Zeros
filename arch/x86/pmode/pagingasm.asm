;Assembly routines for dealing with paging

;Disables paging for the system
global arch_disable_paging
arch_disable_paging:
   push edx
   mov edx, cr0
   and edx, 0x7fffffff
   mov cr0, edx
   pop edx
   ret

;Enables paging. Make sure CR3 has
;teh address for the page_directory_t*!
global arch_enable_paging
arch_enable_paging:
   push edx
   mov edx, cr0
   or edx, 0x80000000
   mov cr0, edx
   pop edx
   ret

;Invalidate a page. Needed whenever we change
;the page mapping without switching out CR3
global inval_page
inval_page:
   invlpg [esp+0x8]
   ret
   

; Used to load a page directory
; First argument is pointer to pd_t*
global load_pd
load_pd:
   ;eax is all that is used
   push eax 
   
   ;Load the address of the page 
   ;structure
   mov eax, [esp+8]
   mov cr3, eax

   call arch_enable_paging

   pop eax
   ret
