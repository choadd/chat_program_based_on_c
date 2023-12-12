# chat_program_based_on_c

## 1. 채팅 프로그램 요구사항

### 1-1. 서버 어플리케이션

a. 채팅 기능
- 1:1 또는 1:N의 채팅 기능 제공

b. 사용자 수용 인원 제한
- 서버는 최대 10명 이상의 사용자를 수용

c. 부가서비스 제공
- 채팅방 개설 기능 제공
- 접속 중인 사용자 리스트를 조회할 수 있는 기능 제공
- 사용자를 강퇴하는 기능 제공

### 1-2. 사용자 유틸리티

a. 채팅방 입장/퇴장
- 채팅방에 입장하거나 퇴장하는 기능 제공

b. 특정 채팅방 입장/퇴장
- 특정 채팅방에 입장하거나 퇴장하는 기능 지원

c. 1:1 메세지 전송
- 특정 개인에게 1:1 메세지를 전송하는 기능 제공

## 2. 채팅 프로그램 폴더 구조 (socket 버전)

- ipc
    - socket
        - Makefile      
        - output
            - outputfile        
        - src           
            - includes  
                - inf
                    - inf_svr_sock.h   
            - inf       
                - inf_svr_sock.c
            server_main.c            
- utils                 
    - chat              
README.md


## 3. 폴더 구조 설명
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