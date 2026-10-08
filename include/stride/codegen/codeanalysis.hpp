#ifndef CODEANALYSIS_HPP
#define CODEANALYSIS_HPP

#include "stridesystem.hpp"

namespace strd {
class CodeAnalysis {
public:
  enum {
    SIZE_UNKNOWN = -1,
    SIZE_MEMBER_ACCESS = -2,
    SIZE_PORT_PROPERTY = SIZE_MEMBER_ACCESS
  };
  // Global tree analysis
  static std::vector<std::string> getUsedDomains(ASTNode tree);
  static std::vector<std::string> getUsedFrameworks(ASTNode tree);

  // Get instance. For entities the declaration is the instance, for modules it
  // is the function node in the stream Many properties need to be stored in the
  // instance.
  static ASTNode getInstance(ASTNode block, ScopeStack scopeStack,
                             ASTNode tree);

  // Retrieve stored declaration in "declaration" compiler property
  static std::shared_ptr<DeclarationNode> getDeclaration(ASTNode node);

  // Domain queries
  static std::shared_ptr<DeclarationNode>
  findDomainDeclaration(std::string domainName, std::string framework,
                        ASTNode tree);

  static std::shared_ptr<DeclarationNode>
  findDomainDeclaration(std::string domainId, ASTNode tree);

  static ASTNode getNodeDomain(ASTNode node, ScopeStack scopeStack,
                               ASTNode tree);

  static std::string getNodeDomainName(ASTNode node, ScopeStack scopeStack,
                                       ASTNode tree);

  static std::string getDomainIdentifier(ASTNode domain, ScopeStack scopeStack,
                                         ASTNode tree);

  static ASTNode resolveDomain(ASTNode node, ScopeStack scopeStack,
                               ASTNode tree, bool downStream = true);

  // Rate queries
  static double getNodeRate(ASTNode node, ScopeStack scope = ScopeStack(),
                            ASTNode tree = nullptr);

  static double resolveRate(ASTNode node, ScopeStack scopeStack, ASTNode tree,
                            bool downStream = true);

  static double resolveRateToFloat(ASTNode rateNode, ScopeStack scope,
                                   ASTNode tree,
                                   std::vector<LangError> *errors);
  static double
  getDomainDefaultRate(std::shared_ptr<DeclarationNode> domainDecl);

  // Framework queries
  static std::string getFrameworkForDomain(std::string domainName,
                                           ASTNode tree);
  // Stream analysis

  /// Number of parallel streams that a single stream can be broken up into
  static int numParallelStreams(StreamNode *stream, StrideSystem &platform,
                                const ScopeStack &scope, ASTNode tree,
                                std::vector<LangError> *errors = nullptr);

  static std::shared_ptr<DeclarationNode>
  resolveConnectionEntity(ASTNode node, ScopeStack scopeStack, ASTNode tree,
                          bool downStream = true);
  static std::shared_ptr<DeclarationNode>
  resolveConnectionBlock(ASTNode node, ScopeStack scopeStack, ASTNode tree,
                         bool downStream = true) {
    return resolveConnectionEntity(node, scopeStack, tree, downStream);
  }

  //
  static ASTNode
  getMatchedOuterInstance(std::shared_ptr<FunctionNode> functionNode,
                          std::shared_ptr<DeclarationNode> blockDecl,
                          std::shared_ptr<DeclarationNode> funcDecl,
                          ScopeStack scopeStack, ASTNode tree);

  static std::vector<ASTNode>
  getEntitiesInScope(ASTNode root, ScopeStack scopeStack, ASTNode tree);
  static std::vector<ASTNode>
  getBlocksInScope(ASTNode root, ScopeStack scopeStack, ASTNode tree) {
    return getEntitiesInScope(root, scopeStack, tree);
  }

  // Evaluate value of member access / port property (size and rate)
  static int evaluateSizeMemberAccess(std::string targetPortName,
                                      ScopeStack scopeStack,
                                      std::shared_ptr<DeclarationNode> decl,
                                      std::shared_ptr<FunctionNode> func,
                                      ASTNode tree);
  static int evaluateSizePortProperty(std::string targetPortName,
                                      ScopeStack scopeStack,
                                      std::shared_ptr<DeclarationNode> decl,
                                      std::shared_ptr<FunctionNode> func,
                                      ASTNode tree) {
    return evaluateSizeMemberAccess(targetPortName, scopeStack, decl, func,
                                    tree);
  }

  static double evaluateRateMemberAccess(std::string targetPortName,
                                         ScopeStack scopeStack,
                                         std::shared_ptr<DeclarationNode> decl,
                                         std::shared_ptr<FunctionNode> func,
                                         ASTNode tree);
  static double evaluateRatePortProperty(std::string targetPortName,
                                         ScopeStack scopeStack,
                                         std::shared_ptr<DeclarationNode> decl,
                                         std::shared_ptr<FunctionNode> func,
                                         ASTNode tree) {
    return evaluateRateMemberAccess(targetPortName, scopeStack, decl, func,
                                    tree);
  }

  static std::vector<std::shared_ptr<MemberAccessNode>>
  getUsedMemberAccesses(std::shared_ptr<DeclarationNode> funcDecl);
  static std::vector<std::shared_ptr<MemberAccessNode>>
  getUsedPortProperties(std::shared_ptr<DeclarationNode> funcDecl) {
    return getUsedMemberAccesses(funcDecl);
  }

  static std::vector<std::shared_ptr<MemberAccessNode>>
  getUsedMemberAccessesInNode(ASTNode node);
  static std::vector<std::shared_ptr<MemberAccessNode>>
  getUsedPortPropertiesInNode(ASTNode node) {
    return getUsedMemberAccessesInNode(node);
  }

  static std::vector<ASTNode> getUsedEntitiesInNode(ASTNode node);
  static std::vector<ASTNode> getUsedBlocksInNode(ASTNode node) {
    return getUsedEntitiesInNode(node);
  }

  // //

  // Input and output size
  static int getNodeNumOutputs(ASTNode node, const ScopeStack &scope,
                               ASTNode tree,
                               std::vector<LangError> *errors = nullptr);
  static int getNodeNumInputs(ASTNode node, ScopeStack scope, ASTNode tree,
                              std::vector<LangError> *errors = nullptr);

  // A value of -1 means undefined. -2 means set from member access / port
  // property.
  static int
  getTypeNumOutputs(std::shared_ptr<DeclarationNode> blockDeclaration,
                    const ScopeStack &scope, ASTNode tree,
                    std::vector<LangError> *errors = nullptr);

  static int getTypeNumInputs(std::shared_ptr<DeclarationNode> blockDeclaration,
                              const ScopeStack &scope, ASTNode tree,
                              std::vector<LangError> *errors = nullptr);

  static int getFunctionDataSize(std::shared_ptr<FunctionNode> func,
                                 ScopeStack scope, ASTNode tree,
                                 std::vector<LangError> *errors = nullptr);

  static int getFunctionNumInstances(std::shared_ptr<FunctionNode> func,
                                     ScopeStack scope, ASTNode tree,
                                     std::vector<LangError> *errors = nullptr);

  static std::shared_ptr<DeclarationNode>
  findDataTypeDeclaration(std::string dataTypeName, ASTNode tree);

  static std::vector<ASTNode>
  getInputDataTypes(ASTNode node, const ScopeStack &scope, ASTNode tree);
  static std::vector<ASTNode>
  getOutputDataTypes(ASTNode node, const ScopeStack &scope, ASTNode tree);
  static ASTNode
  getDataTypeForSignalDeclaration(std::shared_ptr<DeclarationNode> decl);
  static std::string
  getDataTypeForDeclaration(std::shared_ptr<DeclarationNode> decl,
                            const ScopeStack &scope, ASTNode tree);

  static std::string resolveEntityDataType(ASTNode name, ScopeStack scopeStack,
                                           ASTNode tree);
  static std::string resolveBlockDataType(ASTNode name, ScopeStack scopeStack,
                                          ASTNode tree) {
    return resolveEntityDataType(name, scopeStack, tree);
  }
  static std::string resolveNodeOutDataType(ASTNode node, ScopeStack scopeStack,
                                            ASTNode tree);
  static std::string resolveNodeOutDataTypeInternal(ASTNode node,
                                                    ScopeStack scopeStack,
                                                    ASTNode tree);
  static std::string resolveListDataType(ListNode *listnode,
                                         ScopeStack scopeStack, ASTNode tree);
  static std::string resolveExpressionDataType(ExpressionNode *exprnode,
                                               ScopeStack scopeStack,
                                               ASTNode tree);
  static std::string resolveRangeDataType(RangeNode *rangenode,
                                          ScopeStack scopeStack, ASTNode tree);
  static std::string resolveMemberAccessDataType(MemberAccessNode *memberAccess,
                                                 ScopeStack scopeStack,
                                                 ASTNode tree);
  static std::string resolvePortPropertyDataType(PortPropertyNode *portproperty,
                                                 ScopeStack scopeStack,
                                                 ASTNode tree) {
    return resolveMemberAccessDataType(portproperty, scopeStack, tree);
  }

  static std::vector<ASTNode> getDomainIOEntityDeclarations(ASTNode tree);
  static std::vector<ASTNode> getDomainIOBlockDeclarations(ASTNode tree) {
    return getDomainIOEntityDeclarations(tree);
  }

  static std::shared_ptr<DeclarationNode>
  matchDefinitionToTypes(std::vector<std::shared_ptr<DeclarationNode>> decls,
                         std::shared_ptr<FunctionNode> func,
                         const ScopeStack &scope = {}, ASTNode tree = nullptr);

  enum class NodeRole {
    Input,
    Output,
    Internal,
    Persistent,
    External,
    Unknown
  };

  static NodeRole
  determineNodeRole(ASTNode block, std::shared_ptr<DeclarationNode> funcDecl,
                    const ScopeStack &scope, ASTNode tree,
                    std::shared_ptr<DeclarationNode> &outBlockDecl);

  static std::vector<ASTNode>
  getUsedEntitiesInStreams(std::shared_ptr<DeclarationNode> funcDecl);
  static std::vector<ASTNode>
  getUsedBlocksInStreams(std::shared_ptr<DeclarationNode> funcDecl) {
    return getUsedEntitiesInStreams(funcDecl);
  }

  using DataInfo = std::pair<ASTNode, std::string>; // Instance, Type
  struct TypeTree {
    ASTNode instance;
    std::vector<TypeTree> nodes;
    // External refers to data declared above this scope.
    std::vector<DataInfo> input;
    std::vector<DataInfo> output;
    std::vector<DataInfo> external;
    std::vector<DataInfo> persistent; // store state that persistes across calls
    std::vector<DataInfo> internal;

    bool contains(std::string name);
    CodeAnalysis::TypeTree *find(ASTNode node,
                                 CodeAnalysis::TypeTree *tree = nullptr);
    TypeTree *getDomainRootTree(std::string domainName);
    TypeTree *getParentTreeForNode(ASTNode node,
                                   CodeAnalysis::TypeTree *tree = nullptr);
  };

  static TypeTree getStateStructInformation(const ScopeStack &scope,
                                            ASTNode tree);

  static TypeTree
  getStateStructInformationForDeclaration(std::shared_ptr<DeclarationNode> decl,
                                          const ScopeStack &scope,
                                          ASTNode tree);

private:
  static TypeTree
  getTypeTreeForFunctionInstance(std::shared_ptr<FunctionNode> func,
                                 const ScopeStack &scope, ASTNode tree,
                                 int &instanceCounter);
  static void processStreamNode(ASTNode streamNode, const ScopeStack &scope,
                                ASTNode tree, TypeTree &typeTree);
  static void processStreamNodeForDeclaration(
      ASTNode node, std::shared_ptr<DeclarationNode> funcDecl,
      const ScopeStack &funcScope, ASTNode tree, TypeTree &typeTree);
};
} // namespace strd

#endif // CODEANALYSIS_HPP
