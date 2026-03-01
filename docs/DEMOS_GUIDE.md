# Demo 学习指引（建议顺序）

## DEMO 1：调度器与任务状态
观察 Ready/Running/Blocked 的切换：`vTaskDelay()` 进入 Blocked，时间到后回 Ready。

## DEMO 2：饿死现象
高优先级忙等会导致低优先级任务几乎跑不到。改写 hog_task：在循环里加 `vTaskDelay(1)` 再观察差异。

## DEMO 3：delay vs delayUntil
对比周期漂移：`vTaskDelay()` 会把“执行耗时”叠加到周期；`vTaskDelayUntil()` 让周期更稳定。

## DEMO 4：Queue 采样→上传
这是你最终项目最像的形态：采样任务高优先级，上传任务低优先级，队列做缓冲与解耦。

## DEMO 5：Task Notification
理解为什么它比队列更轻量：适合 ISR/任务间快速通知、计数事件、信号量替代等。

## DEMO 6：EventGroup
用事件位门控业务：等 WiFi/MQTT ready 再上传。未来可替换为真实 esp_event + mqtt 事件。

## DEMO 7：优先级反转/继承
观察 High 等待 mutex 的时间。对比：
- 用 mutex（支持优先级继承）
- 你也可以自行把 `xSemaphoreCreateMutex()` 改成 binary semaphore（不继承）观察差异。

## DEMO 8：Software Timer
明确 timer 回调执行环境与约束：不要阻塞、不要做重活。

## DEMO 9：诊断
学会随手抓“系统快照”：任务列表、运行时间统计、heap 水位、stack 余量。
