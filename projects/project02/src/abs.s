.globl abs

.text
# =================================================================
# FUNCTION: Given an int return its absolute value.
# Arguments:
#   a0 (int*) is a pointer to the input integer
# Returns:
#   None
# =================================================================
abs:
    # 读取整数
    lw t0, 0(a0)      
    # 如果是负数，跳转  
    blt t0, x0, make_pos 
    # 否则跳过修改
    j end               

make_pos:
    # 取反
    neg t0, t0         

end:
    # 写回结果
    sw t0, 0(a0)     
    # 返回   
    jr ra              


