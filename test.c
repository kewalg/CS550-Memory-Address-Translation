#include <linux/init.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <asm/current.h>
#include <linux/cred.h>
#include <linux/mm_types.h>
#include <asm/pgtable.h>

MODULE_LICENSE("GPL");
static int process_ID;
module_param(process_ID, int, S_IRUGO);
// called when module is installed
int __init init_module()
{
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *ptep,pte;
	int PFN;

	struct pid *pid;
	struct task_struct *pid_struct;
	struct mm_struct *pid_mm_struct;
	struct vm_area_struct *vma;
	unsigned long vaddr;


	pid = find_get_pid(process_ID);
	pid_struct = pid_task(pid,PIDTYPE_PID);
	pid_mm_struct = pid_struct->mm;

	for(vma = pid_mm_struct->mmap;vma; vma = vma->vm_next){
		for(vaddr = vma->vm_start; vaddr<vma->vm_end; vaddr++){
			pgd = pgd_offset(pid_mm_struct,vaddr);
			printk("pgd_val = 0x%lx\n", pgd_val(*pgd));
			p4d = p4d_offset(pgd,vaddr);
			printk("p4d_val = 0x%lx\n", p4d_val(*p4d));
			pud = pud_offset(p4d,vaddr);
		    printk("pud_val = 0x%lx\n", pud_val(*pud));
			pmd = pmd_offset(pud,vaddr);
			printk("pmd_val = 0x%lx\n", pmd_val(*pmd));
			ptep = pte_offset_map(pmd,vaddr);
			printk("pte_val = 0x%lx\n", pte_val(*ptep));
			pte = *ptep;
			if (pte_present(pte)){
				PFN = pte_pfn(pte);
				printk(KERN_ALERT "PFN= %d\n", PFN);
				return 0;
			}		
		}
	}

	printk(KERN_ALERT "mymodule: Process ID: %d!\n", process_ID);

	return 0;
}



// called when module is removed
void __exit cleanup_module()
{
	//int i;

	//for( i=0; i<howmany; i++)
	printk(KERN_ALERT "mymodule: Goodbye, Process ID: %d!!\n",process_ID);
}
