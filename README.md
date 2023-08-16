# 软件工程项目：地铁漫游

Contributor: GoodMorningPeter smzzl

## 零、任务

使用北京地铁交通图，提供地铁换乘服务。

![](https://www.bjsubway.com/subway/images/subway_map.jpg)

## 一、作业要求

1. 阅读个人软件开发流程（PSP）的相关资料。
2. 可选的语言包括:C++，Java，Python。运行环境为 64bit Windows 10 或 MacOS 12。
3. 提交的代码要求经过代码质量分析工具的分析并消除所有的警告。如[Code Quality Analysis](http://msdn.microsoft.com/en-us/library/dd264897.aspx)。
4. 完成项目的首个版本之后，请使用**性能分析工具**来找出代码中的性能瓶颈并进行改进。
5. 使用单元测试对项目进行测试，并查看测试分支覆盖率等指标；并写出至少 10 个测试用例确保你的程序能够正确处理各种情况。如[Studio Profiling Tools](https://msdn.microsoft.com/en-us/library/mt210448.aspx)。
6. 使用 Git 来管理源代码和测试用例，**代码有进展即签入本地仓库，定期推送到服务器上，并发送合并请求提交每周的工作成果。签入记录不合理的项目会被抽查询问项目细节。**
7. 按照要求发布博客，结合结对项目的实践经历，撰写解决项目的心路历程与收获。**博客与项目明显不符的作业将取消作业成绩。**

> 注意：要求 3、4、5 根据所选编程语言使用对应的开发工具来完成。

## 二、需求

**共 4 个阶段，每周更新**

## 三、博客撰写要求

使用 Markdown 编写小组博客，可在小组目录中建立一个名为 blog 的子目录，建议每周一个文件，描述一周的工作进展。

- **PSP 表格**使用
  - 每个迭代开始之前，需要使用该表格进行时间估计
  - 每个迭代进行过程中，需要使用该表格进行时间记录
  - 每个迭代结束之后，使用该表格对比预估时间和实际时间，分析其产生差别的原因
- 在开始实现程序之前，在**PSP 表格**记录下你估计的时间。
- 看教科书和其它资料中关于 Information Hiding, Interface Design, Loose Coupling 的章节，说明你们在结对编程中是如何利用这些方法对接口进行设计的。
- 描述重要**模块接口的设计与实现过程**。设计包括代码如何组织，比如会有几个类，几个函数，他们之间关系如何，关键函数是否需要画出流程图？说明你的算法的关键（不必列出源代码），以及独到之处。
- 阅读有关 UML 的内容：[UML](https://en.wikipedia.org/wiki/Unified_Modeling_Language)。 画出 UML 图显示计算模块部分各个实体之间的关系(画一个图即可)。
- 看 Design by Contract, Code Contract 的内容：
  [Design by contract](http://en.wikipedia.org/wiki/Design_by_contract)
  [Code contract](http://msdn.microsoft.com/en-us/devlabs/dd491992.aspx)
  描述这些做法的优缺点, 说明你是如何把它们融入结对作业中的。
- 程序的代码规范，设计规范。 你们两如何达成共识，采用了什么规范？ 程序中是否有异常处理？你是如何处理各种异常的？
- **界面模块的详细设计过程**：你的程序有用户界面么？在博客中详细介绍界面模块是如何设计的。
- **界面模块与其它模块的对接**：详细地描述 UI 模块的设计与其他模块的对接，并在博客中截图实现的功能。界面/控制/数据模块体现了 [MVC 的设计模式](https://en.wikipedia.org/wiki/Model–view–controller)了么？
- **描述结对的过程**，提供非摆拍的两人在讨论的结对照片。可以做遮挡或美化。
- 看教科书和其它参考书，网站中关于[结对编程的章节](http://www.cnblogs.com/xinz/archive/2011/08/07/2130332.html)，说明你们采用了哪种合作方式，以及结对编程的优点和缺点。
  - 结对的每一个人的优点和缺点在哪里 (要列出至少三个优点和一个缺点)。
  - 你如何说服你的伙伴改进 TA 的缺点？请考虑一下三明治方法。
- 在你实现完程序之后，在附录提供的**PSP 表格**记录下你在开发各个步骤上实际花费的时间。并说明差异的原因。
- 其它收获，例如，如何攻克技术难点，你做了哪些阅读，探索，可以把资料和经历描述一下。 如果你的项目是和其他同学一起比赛（例如比赛速度），描述一下你的程序和其他程序的优劣。

## 四、评分规则

结对项目分数由三部分组成，分别是

- 博客 — 25 分。
- 程序 — 25 分

```
5分为源代码管理评分，该评分主要通过源代码管理中的commit注释信息，增量修改的内容，是否有运行说明等给分。
15分为正确性评分。
5分为性能评分。
当程序的正确性评分等于15分时才可以参与性能评分环节，所以请各位同学务必保证自己程序的正确性。
```

- 注意事项：

```
按时间完成并提交——正常评分
晚交一周以内——折扣90%
晚交一周以上——折扣70%
不交或抄袭——0分【严禁代码与博客等一切形式的抄袭！请各位同学千万不要触碰底线，勿谓言之不预也！】
```

_附：PSP 表格_

| PSP                                     | Personal Software Process Stages        | 预估耗时（分钟） | 实际耗时（分钟） |
| --------------------------------------- | --------------------------------------- | ---------------- | ---------------- |
| Planning                                | 计划                                    |                  |                  |
| · Estimate                              | · 估计这个任务需要多少时间              |                 |                 |
| Development                             | 开发                                    |                  |                  |
| · Analysis                              | · 需求分析 (包括学习新技术)             |  |                  |
| · Design Spec                           | · 生成设计文档                          | |                  |
| · Design Review                         | · 设计复审 (和同事审核设计文档)         |  |                  |
| · Coding Standard                       | · 代码规范 (为目前的开发制定合适的规范) |  |                  |
| · Design                                | · 具体设计                              |  |                  |
| · Coding                                | · 具体编码                              |  |                  |
| · Code Review                           | · 代码复审                              |  |                  |
| · Test                                  | · 测试（自我测试，修改代码，提交修改）  |  |                  |
| Reporting                               | 报告                                    |                  |                  |
| · Test Report                           | · 测试报告                              |  |                  |
| · Size Measurement                      | · 计算工作量                            |  |                  |
| · Postmortem & Process Improvement Plan | · 事后总结, 并提出过程改进计划          |  |                  |
|                                         | 合计                                    |  |                  |
