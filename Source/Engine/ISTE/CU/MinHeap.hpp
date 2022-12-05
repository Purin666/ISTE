#pragma once
#pragma once
#include <vector>


namespace CU
{
	template<class T>
	class MinHeap
	{
	public:
		MinHeap() = default;
		~MinHeap() = default;

		void Clear()
		{
			myContainer.clear();
		}
		
		void Reserve(int aReserveSize)
		{
			myContainer.reserve(aReserveSize);
		}

		int GetSize() const
		{
			return (int)myContainer.size();
		}

		//lägger till elementet i heapen
		void Enqueue(const T& aElement)
		{
			myContainer.emplace_back(aElement);
			BubbleUp((int)myContainer.size() - 1);
		}

		//returnerar det största elementet i heapen
		const T& GetTop() const
		{
			return myContainer[0];
		}

		//tar bort det största elementet ur heapen och returnerar det
		T Dequeue()
		{
			T toReturn = myContainer[0];
			myContainer[0] = myContainer.back();
			myContainer.pop_back();
			BubbleDown(0);
			return toReturn;
		}


	private:
		void BubbleUp(int aIndex)
		{
			if (aIndex < 0)
				return;

			if (myContainer[aIndex] < myContainer[(aIndex - 1) / 2])
			{
				T currentNode = myContainer[aIndex];
				T parentNode = myContainer[(aIndex - 1) / 2];

				myContainer[aIndex] = parentNode;
				myContainer[(aIndex - 1) / 2] = currentNode;

				BubbleUp((aIndex - 1) / 2);
			}
		}
		void BubbleDown(int aIndex)
		{

			int largerValue = aIndex;
			int firstChildIndex = 2 * aIndex + 1;
			int secondChildIndex = 2 * aIndex + 2;

			if ((2 * aIndex + 1 < (int)myContainer.size()) && myContainer[firstChildIndex] < myContainer[largerValue])
			{
				largerValue = firstChildIndex;
			}
			if ((2 * aIndex + 2 < (int)myContainer.size()) && myContainer[secondChildIndex] < myContainer[largerValue])
			{
				largerValue = secondChildIndex;
			}
			if (largerValue != aIndex)
			{
				T valueToBubbleUp = myContainer[largerValue];
				T valueToBubbleDown = myContainer[aIndex];

				myContainer[aIndex] = valueToBubbleUp;
				myContainer[largerValue] = valueToBubbleDown;
				BubbleDown(largerValue);
			}
			return;
		}


		std::vector<T> myContainer;
	};
}