import { Link } from "react-router-dom";

export default function Navbar() {
  return (
    <nav style={{ padding: "10px", borderBottom: "1px solid black" }}>
      <Link to="/">Dashboard</Link>{" | "}
      <Link to="/register">Register User</Link>{" | "}
      <Link to="/create">Register IP</Link>{" | "}
      <Link to="/verify">Verify Ownership</Link>{" | "}
      <Link to="/chain">Blockchain</Link>
    </nav>
  );
}
