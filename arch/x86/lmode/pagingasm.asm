;Assembly routines related to paging

;Invalidates a page. Needed when a page
;mapping has changed, but CR3 has not 
;been altered
global inval_page
inval_page:
   invlpg [rsp+0x10]
   ret

;Load address of pml4 into cr3
global load_pml4
load_pml4:
   ;load address of page structure
   mov rax, [rsp+0x10]
   mov cr3, rax
 
   ret 

