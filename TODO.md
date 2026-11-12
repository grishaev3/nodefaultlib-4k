# TODO

Пункты нумеруются по разделам: B — ошибки, P — производительность, J — мусор в репозитории,
S — сборка и инфраструктура. Номер пункта пишется в сообщении коммита
(`B1: ...`), чтобы находить связанные изменения через `git log --grep B1`.
Закрытый пункт не удаляется: отмечается `[x]` и хэшем коммита.

Пути указаны относительно корня репозитория, исходники лежат в `src/nodefaultlib-4k/`.

## Ошибки

- [ ] **B1. Свет не работает.**
  [lext.h:123](src/nodefaultlib-4k/lext.h#L123): `l_Uniform4fv` передаёт в
  `glGetUniformLocation` индекс `prgIndex` (0), а не `g_shadingProgram[prgIndex]`.
  Вдобавок `u_LightPos` в шейдере — `vec3`, а задаётся через `glUniform4fv`
  ([main.c:286](src/nodefaultlib-4k/main.c#L286)), что даёт `GL_INVALID_OPERATION`.
- [ ] **B2. Нормали не доходят до шейдера.**
  [draw.h:94-98](src/nodefaultlib-4k/draw.h#L94-L98) подаёт вершины через устаревшие
  `glVertexPointer`/`glNormalPointer`, а шейдер ждёт `layout(location=0/1)`.
  NVIDIA связывает `gl_Normal` с атрибутом 2, поэтому `a_normal` получает мусор или ноль;
  это скрыто ограничением `max(dot, 0.4)` в [f.hlsl](shd/f.hlsl). На AMD/Intel картинки
  может не быть вовсе. Нужны `glVertexAttribPointer` + `glEnableVertexAttribArray`.
- [ ] **B3. Шейдеры не соответствуют своей версии.** В `#version 330 core` используются
  `varying` и `gl_FragColor` ([v.hlsl](shd/v.hlsl), [f.hlsl](shd/f.hlsl),
  [wire_f.hlsl](shd/wire_f.hlsl)), которых в core-профиле нет. Строгие драйверы такие
  шейдеры отклонят. Нужны `in`/`out` и свой выход фрагментного шейдера.
- [ ] **B4. Нормали искажаются масштабом.** [v.hlsl:25](shd/v.hlsl#L25) умножает нормаль
  на `view * world`, а `world` содержит неравномерный масштаб блока. Нужна матрица
  нормалей (обратная транспонированная) или нормализация в нужном пространстве.
- [ ] **B5. Освобождается системная куча процесса.**
  [lmemory.h:48](src/nodefaultlib-4k/lmemory.h#L48): `HeapDestroy(GetProcessHeap())` —
  неопределённое поведение. Достаточно не вызывать его или освобождать свои блоки.
- [ ] **B6. `check_shader` не выдаёт ошибок компиляции.**
  [lext.h:267](src/nodefaultlib-4k/lext.h#L267): `glGetShaderiv(shader, maxLength, ...)`
  передаёт 260 вместо `GL_INFO_LOG_LENGTH`; сам лог никуда не выводится.
- [ ] **B7. `load_shaders` не замечает сбоев чтения.**
  [io.h:127](src/nodefaultlib-4k/io.h#L127) проверяет указатели, а не результат
  `read_file`. Буфер жёстко 1024 байта, `read_file` не дописывает завершающий ноль
  (спасает только `HEAP_ZERO_MEMORY`). [io.h:101](src/nodefaultlib-4k/io.h#L101)
  возвращает `NULL` из функции типа `size_t`.
- [ ] **B8. Имя класса окна работает случайно.**
  [main.c:363](src/nodefaultlib-4k/main.c#L363): `L"minwindowsapp"` при кодировке
  MultiByte и `CreateWindowA` даёт класс `"m"`. Работает только потому, что оба вызова
  получают один указатель.
- [ ] **B9. Флаг `IS_SHOW_SIZE_CAPTION` ломает Release.**
  [main.c:373](src/nodefaultlib-4k/main.c#L373) использует `strcat`/`strlen`, а CRT
  в Release отключён — компоновка упадёт.
- [ ] **B10. Тоннель перестраивается рывком.**
  [main.c:265](src/nodefaultlib-4k/main.c#L265): `currPosZ` перезаписывается на каждой
  итерации, в нём остаётся позиция только последнего кольца. Когда оно проходит порог,
  заново генерируется весь тоннель, а не по одному кольцу.
- [ ] **B11. `get_transform` объявлена возвращающей `mat4_t`, но ничего не возвращает,**
  и умножает матрицу саму на себя: `m4_mul(result, result, scaling)`
  ([math_3d.h:374-379](src/nodefaultlib-4k/math_3d.h#L374-L379)). Результат верен только
  потому, что матрица масштаба диагональная; с любой другой матрицей сломается.
- [ ] **B12. Мелочи.** `on_exit` вызывается и в `WM_DESTROY`, и после цикла
  ([main.c:406](src/nodefaultlib-4k/main.c#L406), [main.c:413](src/nodefaultlib-4k/main.c#L413));
  `glViewport` и `PeekMessage` получают `0.0f` вместо целых
  ([main.c:144](src/nodefaultlib-4k/main.c#L144), [main.c:392](src/nodefaultlib-4k/main.c#L392));
  `test_random_numbers` не возвращает значение ([tests.h:56](src/nodefaultlib-4k/tests.h#L56)).

## Производительность

- [ ] **P1. Около 4300 вызовов отрисовки за кадр.** ~2160 кубов × 2 прохода
  (заливка + каркас), каждый вызов заново привязывает три буфера и включает client state
  ([draw.h:90-106](src/nodefaultlib-4k/draw.h#L90-L106)). Варианты: привязать VAO один раз,
  инстансинг с буфером матриц. Учитывать цену в байтах.
- [ ] **P2. `glGetUniformLocation` на каждый вызов отрисовки.**
  [geom.h:73](src/nodefaultlib-4k/geom.h#L73), [s_geom.h:59](src/nodefaultlib-4k/s_geom.h#L59):
  поиск uniform по строке ~4300 раз за кадр. Достаточно получить location один раз
  после линковки программы.
- [ ] **P3. Кадр ограничивается трижды.** `wglSwapInterval(1)` вызывается каждый кадр
  ([main.c:315](src/nodefaultlib-4k/main.c#L315)), плюс `Sleep` в главном цикле, плюс
  `glFinish` после `SwapBuffers` ([main.c:317](src/nodefaultlib-4k/main.c#L317)).
  Оставить vsync (один раз при инициализации) и убрать остальное.

## Мусор в репозитории

- [x] **J1. Дубликаты `crinkler.exe`** (3 копии + переименованная в `link.exe`) — 94d73a9
- [x] **J2. Кэш IntelliSense `*.ipch` в отслеживаемых файлах** — 4a5441e
- [ ] **J3. Устаревший [shader_code.h](src/nodefaultlib-4k/shader_code.h)** — нигде не
  подключается, дублирует `fhlsl.h`.
- [ ] **J4. Неиспользуемый код:** [args.h](src/nodefaultlib-4k/args.h),
  `get_pixel_format` ([lext.h:206](src/nodefaultlib-4k/lext.h#L206)), пустые `Q_LOW`/`Q_HI`
  ([main.c:14-15](src/nodefaultlib-4k/main.c#L14-L15)), закомментированный код в `main.c`.
- [ ] **J5. Неиспользуемые бинарники:** [pkg/upx.exe](pkg/upx.exe) и пустой
  [pkg/upx_after_build.cmd](pkg/upx_after_build.cmd); `stub.bin`, `debug.txt` в папке
  исходников.
- [ ] **J6. Комментарии в cp1251.** Часть комментариев в `main.c` (и, возможно, других
  файлах) в кодировке Windows-1251 — в редакторах с UTF-8 отображаются «кракозябрами».
  Перекодировать исходники в UTF-8.
- [ ] **J7. Конфигурации x64 нерабочие.** Код использует `__asm` (x87), который не
  поддерживается в x64; конфигурации `Debug|x64`/`Release|x64` в проекте не собираются.
  Удалить их или заменить ассемблер на интринсики.
- [ ] **J8. Папка `.hg` и `.hgignore`.** Старый репозиторий Mercurial, после проверки
  миграции можно удалить.
- [ ] **J9. Тяжёлые файлы в истории.** `*.ipch` (~80 МБ) остались в старых коммитах.
  Если размер репозитория важен — переписать историю без них (до первого push).

## Сборка и инфраструктура

- [x] **S1. Переход с Mercurial на git** с сохранением истории (71 коммит, 3 ветки) — 4a5441e
- [x] **S2. Не находились `glext.h`/`wglext.h`** — путь `inc/` в проекте — 477b1b3
- [x] **S3. Release не компоновался под VS 2026** (Crinkler не подключался, нет `memset`) — ee1a277
- [x] **S4. Crinkler 2.0 → 3.0b**, итоговый размер 3444 байта — 0d0c510
- [x] **S5. Проект переименован в `nodefaultlib-4k`** — f2910d2
- [ ] **S6. Push на GitHub** (`origin` = github.com/grishaev3/nodefaultlib-4k).
  Сейчас 403: git авторизован как `IlyaGrishaev`, репозиторий принадлежит `grishaev3`.
- [ ] **S7. Предупреждения компилятора.** Несовместимые типы указателей
  (`mat4_t*` → `GLfloat*`), потеря точности `double` → `float`, функции без `return`.
