# fire_fighter_robot
nextTurn = check hiện tại đang rẽ phải ? phải? <br />
<space>obstacle1 = ahead,obstacle2 = left,obstacle3 = right;
---
- moveForward(): check lửa ở 3 hướng bằng sensor và tính khoảng cách phía trước bằng sensorAhead nếu <19 thì dừng xe và chờ 2s nếu dừng quá 2s thì dừng chương trình.<br />
- Explain automatic()<br />
=> #0: nếu có vật cản phía trước -> dừng xe -> tiếp tục kiểm tra "cản bên phải ko?" và "ko cản bên trái" return TRUE -> đến #1 RẼ TRÁI. <br />
=> vào #1: 
#2: bên trái có vật cản và bên phải ko có vật cản => rẽ phải <br />
<space>#3: cả 2 bên trái phải đều có vật cản => lùi lại => bên phải có vật cản? rẽ trái, delay(leftDelay)=> đi thẳng rồi rẽ phải.<br />
bên trái có vật cản? => rẽ phải, lùi lại, rẽ trái <br />
<space>#4:<br />

