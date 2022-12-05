//#include "PositionCommand.h"
//#include "ISTE/Scene/SceneHandler.h"
//#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
//
//PositionCommand::PositionCommand(T* aDestination, T aOrigin, T aResult)
//	: myEntity(anEntity)
//	, myOrigin(aOrigin)
//	, myResult(aResult)
//{
//
//}
//
//PositionCommand::~PositionCommand() {
//
//}
//
//bool PositionCommand::Execute() {
//	aDestination = myResult;
//	return false;
//}
//
//bool PositionCommand::Undo() {
//	ISTE::TransformComponent* tempTransform = ISTE::Context::Get()->mySceneHandler->GetActiveScene().GetComponent<ISTE::TransformComponent>(myEntity.myId);
//	tempTransform->myPosition = myOrigin;
//	return false;
//}