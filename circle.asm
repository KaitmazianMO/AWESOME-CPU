    push 300
    pop rhx             ; circle radius  

    push 3.1415
    push rhx
    div
    pop rdx             ; delta fi

    push -3.1415
    pop rcx
circle_loop:            ; range from -PI to PI

    push rcx            ; x = r * cos (fi)
    cos
    push rhx
    mult
    pop rax
        
    push rcx            ; y = r * sin (fi)
    sin
    push rhx
    mult
    pop rbx

    push (rax, rbx)    ; set pixel

    push rdx           ; increase fi by dx
    push rcx
    add
    pop rcx

    push rcx
    push 3.1415  
    jbe circle_loop



    draw
    end