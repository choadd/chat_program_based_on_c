# chat_program_based_on_c

## 1. 채팅 프로그램 폴더 구조 (socket 버전)

```
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
```


## 2. 폴더 구조 설명
- ipc
    - socket
        - Makefile      : 프로젝트 빌드 Makefile
        - output        : 빌드된 실행파일/결과물 저장 경로
        - src           : 소스코드가 위치하는 경로
            - includes  : 헤더파일이 위치하는 경로
                - inf   : 소켓 인터페이스에 대한 헤더파일
            - inf       : 소켓 인터페이스 관련 소스파일

- utils                 : 사용자 유틸리티 경로
    - chat              : 채팅 관련 유틸리티 파일
