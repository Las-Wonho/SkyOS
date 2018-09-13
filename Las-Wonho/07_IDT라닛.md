# IDT의 구현

잡담이 길었다. 그래서 우리는 이제 IDT를 만들것이다.

GDT에서 만들었던 내용과 비슷하다고 한다.

- GetInterruptDescriptor
    - 특정 디스크립터의 값을 얻어온다.

- InstallInterrputHandler
    - 인터럽트 서비스 루틴을 설치한다.

- IDTInitialize
    - IDT를 초기화한다.

- IDTInstall
    - CPU에 IDT의 위치를 알려준다.

- _idt
    - IDT를 나타낸다. 총 256개의 인터럽트 디스크립터가 존재하며 처음 디스크립터는 항상 NULL로 설정한다.

- _idtr
    - idtr 레지스터에 로드될 값. IDT의 메모리 주소 및 IDT의 크기를 담고 있다.

왜 이걸 GDT때에 알려주지 않았는지 모르겠다. 아래는 인터럽트 핸들러이다. 기본적으로 인터럽트가 발생하면 아래의 코드가 실행된다. 이 함수는 전에 봤던 키워드를 하고있다. 그런데 어셈블리어를 이해 못하겠다.

```c++
__declspec(naked) void InterrputDefaultHandler () {
    //레지스터를 저장하고 인터럽트를 끈다.
    _asm
    {
        PUSHAD //레지스터 값들을 스택에 저장(백업용)
        PUSHFD //플래그 값을 스택에 저장
        CLI //클리어 인터럽트 플래그. 인터럽트중에 예외 발생하지 않게 함(재귀?)
    }
    SendEOI();
    // 레지스터를 복원하고 원래 수행하던 곳으로 돌아간다.
    _asm
    {
        POPFD //스택의 값을 플레그에 저장
        POPAD //스택의 데이터를 레지스터에 저장(복구)
        IRETD //인터럽트 리턴(끝, 32비트 연산자 크기)
    }
}
```

여기서 처음보는 함수가 하나 등장한다. SendEOI();인데 이것의 구현은 아래와 같다. 난 잘 모르겠으니 찾으면 얘기해주길 바란다.

```c++
__declspec(naked) void SendEOI()
{
    _asm
    {
        PUSH EBP
        MOV  EBP, ESP
        PUSH EAX

        ; [EBP] < -EBP
        ; [EBP + 4] < -RET Addr
        ; [EBP + 8] < -IRQ 번호

        MOV AL, 20H; EOI 신호를 보낸다.
        OUT DMA_PICU1, AL

        CMP BYTE PTR[EBP + 8], 7
        JBE END_OF_EOI
        OUT DMA_PICU2, AL; Send to 2 also

        END_OF_EOI :
        POP EAX
            POP EBP
            RET
    }
}
```

## PIC라는 새로운 개념

Programmable Interrupt Controller이며 입력을 처리하기 위하여 필요하다. 그냥 하드웨어인듯.