구조
device.h/.cpp 
-> dvfs.h/.cpp (클래스 상속 받음)
-> record.h/.cpp (include하고 있음)
example.cpp (사용 예시에 대한 예제들을 담고 있음.)

주의점
1. 가장 먼저, 모바일 Termux에서 사용시 `termux-setup-storage`로 파일 접근 권한 줘야함.
2. 대부분의 함수는 dvfs.h/.cpp에서 구현될 것입니다.
    (현재, RAM DVFS는 S24에서만 가능하므로 1차 버전에서는 구현하지 않습니다.)
    (추후 구현시에도 S24에서만 돌아갑니다)
3. record.h/cpp에서도 여러 함수가 존재하지만, 실질적으로 사용할 함수는 record_hard밖에 없습니다.
4. record_hard함수는 인자를 (bool& sigterm, const DVFS& dvfs)로 받습니다.
   이때 sigterm은 reference type으로 background process로 돌아갈 때 계속해서 reference를 체크하면서 sigterm이 true면 record process를 끝냅니다.
   따라서, 이 함수는 무조건 daemon process로 background에서 돌아가게끔 call이 되어야 함.
   그렇지 않으면, 무한 루프 돌거나 main process blocking이 될 수 있습니다.
5. 현재 DVFS 부분은 유니바측이 지닌 모바일에서 안 돌아갈 수도 있습니다.


사용법
- DVFS 객체에서 CPU/Clock frequency를 setting할 수 있습니다.
- DVFS 객체에서는 사용 가능한 cpu_freq/ddr_freq/cluster_indices들을 얻을 수 있습니다.
   (cluster_indices는 cpu index가 cpu0~9로 모바일에서는 존재하는데 prime/gold/silver cluster를 구분하는 cpu index를 의미합니다.)

<CPU DVFS>
1. DVFS 객체를 생성할 때 기기 모델 이름을 넣으면, 모델 이름에 대한 cpu_freq, ddr_freq, cluster_indices들을 자체적으로 load합니다.
2. CPU DVFS를 하고 싶을 때는 각 cluster에 알맞은 cpu_frequency의 index를 넣어야 합니다.
   예를 들어, cpu가 0,4,7의 cluster가 있고, 각각의 cluster가 {100, 200, 300}, {150, 250, 350}, {300, 500, 700}이라고 한다면,
	0 -> {100, 200, 300}
	4 -> {150, 250, 350}
	7 -> {300, 500, 700}
   CPU DVFS를 dvfs.set_cpu_freq({0, 1, 2}) 이렇게 했다고 가정합니다.
   그러면, 0번 cluster cpu는 모두 100으로 fix하고, 4번 cluster cpu는 모두 250, 7번 cluster cpu는 모두 700으로 고정합니다.

<RECORD HARD INFO>
1. background process로 call해야 합니다.
2. 그리고 실험이 끝나면, record_hard 외부에서 인자로 주는 sigterm 변수를 true로 변경해줘야 합니다.
3. 그러면, reference type이므로 값이 sigterm을 record_hard에서 true를 인지하고 함수를 종료합니다.
(4. 현재는 테스트를 위해 record.cpp 부분의 마지막에 tester code가 활성화 되어 있습니다. 주석처리해주시고 사용하시면 됩니다.)

공지
1. [dvfs.cpp] 추가적으로 unset_cpu_freq 함수를 통해 기존에 설정해두었던 cpu_freq을 초기화해주는 함수가 추가 구현될 것입니다.
2. [dvfs.cpp] S24 버전으로 RAM DVFS도 set/unset으로 구현될 것입니다.
3. 수정되는 부분이 있다면, 계속해서 공지드리겠습니다.