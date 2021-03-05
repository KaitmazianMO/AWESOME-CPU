    in
    pop rax

    call fact
    out

    end


fact:
    push rax
    push rax
    push 1
    sub

    pop rbx
    push rbx

    push 2
    push rbx
    ja end
    pop rax
    call fact

end:
    mult 
    ret
