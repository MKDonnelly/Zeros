;Assembly routines related to paging

;Invalidates a page. Needed when a page
;mapping has changed, but CR3 has not 
;been altered
global inval_page
inval_page:
   ;caller passes argument in rdi
   invlpg [rdi]
   ret

;Load address of pml4 into cr3
global load_pml4
load_pml4:
   ;caller passes address in rdi
   mov cr3, rdi
   ret
