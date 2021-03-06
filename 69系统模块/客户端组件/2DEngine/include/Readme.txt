HGE 中文显示解决方案
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Author  - 微妙的平衡(BOGY)
Mail    - bogy.cn@gmail.com
Home    - http://bogy.cn

文件与目录结构说明
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    [FreeType]          矢量字体支持库(FreeType)

    GdiFont.cpp         像素字体实现定义文件

    GdiFont.h           像素字体实现声明文件

    TypeFont.cpp        矢量字体实现定义文件

    TypeFont.h          矢量字体实现声明文件

    Sprites.h           矢量字体实现定义文件

    Readme.txt          本描述文档


简单介绍
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    本方案是为解决在HGE的项目中支持中文的显示。

	目前支持两种方案：

		一、像素字体的显示；

		二、矢量字体的显示。

	这两种字体的创建与显示由类 FontSprite 的派生类实现，所以使用时只需要使用 FontSprite 类中不同的创建函数即可创建像素字体或是矢量字体指针。

	本解决方案不需要依赖任何图片字模，并支持丰富的显示方法。

	使用方法很简单，只需如下几步即可：

		一、包含字体精灵头文件，详细请参阅 <CN> 目录下的 Readme.txt 文件。

		二、创建字体指针：

			像素字体： FontSprite* fontspr = FontSprite::Create(字体名,字体大小)
			矢量字体： FontSprite* fontspr = FontSprite::CreateEx(字体文件名,字体大小)

		三、渲染文本： fontspr->Printf(坐标x,坐标y,文本内容)

		四、销毁字体：当程序结束时，使用 fontspr->Release() 来销毁字体。


声明与感谢
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	在这个方案支持，我一直都是用一种比较烂的方案来显示汉字，例如很久之前使用的静态字模的方式。
	并且像素字体与矢量字体两种实现方式大相径庭，没有通用性。但看过 Kevin Lynx 推荐的一个矢量字体方案后，就将那个框架移植了过来，并做了一部分的改动。
	最重要的是对像素字体与矢量字体做了通用性的修改，使两种字体的解决方案可以互相使用。

	在这里感谢 Kevin Lynx 同学推荐的那个矢量字体方案。

	若你在使用中还有疑问，请至邮：bogy.cn@gmail.com 我会尽快回复。
____________________________________________________________
Copyright 2006-2007 BOGY.CN. All Rights Reserved.
