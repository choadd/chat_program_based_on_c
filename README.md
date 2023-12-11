# chat_program_based_on_c

## 1. 채팅 프로그램 폴더 구조 (socket 버전)

.
├── ipc
│   └── socket
│       ├── Makefile
│       ├── output
│       └── src
│           ├── includes
│           │   └── inf
│           │       └── inf_svr_sock.h
│           ├── inf
│           │   └── inf_svr_sock.c
│           └── server_main.c
├── README.md
└── utils
    └── chat



## 2. 폴더 구조 설명
- ipc
    - socket
        - Makefile      : 프로그램 빌드용
        - output        : 실행파일 저장경로
        - src           
            - includes  : 헤더파일 저장경로
                - inf   : interface 헤더파일
            - inf       : interface 소스코드

- utils                 : 사용자 유틸리티
    - chat              : chat 유틸리티
