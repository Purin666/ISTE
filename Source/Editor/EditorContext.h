#pragma once

namespace ISTE
{

	class BaseEditor;
	class AssetsManager;
	class Gizmos;

	struct EditorContext
	{
	public:
		inline static EditorContext* Get() { return myInstance; }

		BaseEditor*	   myBaseEditor    = nullptr;
		AssetsManager* myAssetsManager = nullptr;
		Gizmos*		   myGizmos		   = nullptr;

	private:
		friend class BaseEditor;
		inline static EditorContext* myInstance;

	};
}